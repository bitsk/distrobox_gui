import subprocess
import json
import shutil
from typing import List, Dict, Optional

class DistroboxManager:
    """Handles interaction with the distrobox CLI."""
    
    @staticmethod
    def is_distrobox_installed() -> bool:
        return shutil.which("distrobox") is not None

    def install_distrobox(self) -> bool:
        """Installs distrobox to ~/.local using the official script."""
        install_cmd = "curl -s https://raw.githubusercontent.com/89luca89/distrobox/main/install | sh -s -- --prefix ~/.local"
        try:
            # We use shell=True here because we are piping commands
            subprocess.run(install_cmd, shell=True, check=True)
            return True
        except subprocess.CalledProcessError:
            return False

    def get_containers(self) -> List[Dict[str, str]]:
        """Returns a list of distrobox containers."""
        if not self.is_distrobox_installed():
            return []

        # Try JSON format first (newer distrobox versions)
        try:
            result = subprocess.run(
                ["distrobox", "list", "--json"], 
                capture_output=True, 
                text=True, 
                check=True
            )
            # If output is empty or whitespace, return empty list
            if not result.stdout.strip():
                return []
            return json.loads(result.stdout)
        except (subprocess.CalledProcessError, json.JSONDecodeError):
            pass # Fallback to text parsing if JSON fails

        # Fallback: Parse simple text output
        # Usually format is: ID | NAME | STATUS | IMAGE
        try:
            result = subprocess.run(
                ["distrobox", "list", "--no-color"], 
                capture_output=True, 
                text=True, 
                check=True
            )
            return self._parse_text_output(result.stdout)
        except subprocess.CalledProcessError:
            return []

    def _parse_text_output(self, output: str) -> List[Dict[str, str]]:
        """Parses the raw text output of 'distrobox list'."""
        lines = output.strip().split('\n')
        if len(lines) < 2: # Header + at least one line
            return []
        
        containers = []
        # Skip header
        for line in lines[1:]:
            parts = [p.strip() for p in line.split('|')]
            if len(parts) >= 3:
                # This is a best-effort mapping, actual columns may vary by version
                containers.append({
                    "id": parts[0],
                    "name": parts[1],
                    "status": parts[2],
                    "image": parts[3] if len(parts) > 3 else "unknown"
                })
        return containers

    def get_icon_name(self, image_name: str) -> str:
        """Returns a standard icon name based on the distro image."""
        image_name = image_name.lower()
        if "ubuntu" in image_name: return "ubuntu"
        if "fedora" in image_name: return "fedora"
        if "arch" in image_name: return "archlinux"
        if "debian" in image_name: return "debian"
        if "alpine" in image_name: return "alpine"
        if "suse" in image_name: return "opensuse"
        if "kalilinux" in image_name or "kali" in image_name: return "kali-linux"
        return "linux-generic" # Fallback

    def create_box(self, name: str, image: str, home_path: str = None, volume: str = None, root: bool = False) -> bool:
        """Creates a new distrobox container with advanced options."""
        cmd = ["distrobox", "create", "-n", name, "-i", image, "-Y"]
        
        if home_path:
            cmd.extend(["--home", home_path])
        
        if volume:
            cmd.extend(["--volume", volume])
            
        if root:
            cmd.append("--root")

        try:
            subprocess.run(cmd, check=True)
            return True
        except subprocess.CalledProcessError:
            return False

    def stop_box(self, name: str) -> bool:
        """Stops a distrobox container."""
        try:
            subprocess.run(["distrobox", "stop", name, "-Y"], check=True)
            return True
        except subprocess.CalledProcessError:
            return False
            
    def delete_box(self, name: str) -> bool:
        """Deletes a distrobox container."""
        try:
            subprocess.run(["distrobox", "rm", name, "-Y"], check=True)
            return True
        except subprocess.CalledProcessError:
            return False

    def enter_box_command(self, name: str) -> List[str]:
        """Returns the command to enter the box."""
        return ["distrobox", "enter", name]