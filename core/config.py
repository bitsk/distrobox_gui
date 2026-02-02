import json
import os
from typing import List

CONFIG_FILE = os.path.join(os.path.dirname(os.path.dirname(__file__)), "settings.json")

DEFAULT_IMAGES = [
    "ubuntu:latest",
    "ubuntu:22.04",
    "fedora:latest",
    "fedora:39",
    "archlinux:latest",
    "debian:stable",
    "alpine:latest",
    "kalilinux/kali-rolling"
]

def load_config() -> dict:
    if not os.path.exists(CONFIG_FILE):
        return {"custom_images": []}
    try:
        with open(CONFIG_FILE, 'r') as f:
            return json.load(f)
    except:
        return {"custom_images": []}

def save_config(config: dict):
    try:
        with open(CONFIG_FILE, 'w') as f:
            json.dump(config, f, indent=4)
    except Exception as e:
        print(f"Failed to save config: {e}")

def get_image_list() -> List[str]:
    """Returns combined list of default and custom images."""
    config = load_config()
    custom = config.get("custom_images", [])
    # Combine and deduplicate, keeping order
    combined = []
    # Add custom ones first (so they appear at top or specifically marked)
    # Or maybe append them. Let's append them but ensure no duplicates.
    
    seen = set()
    
    # Let's put defaults first, then custom
    for img in DEFAULT_IMAGES:
        if img not in seen:
            combined.append(img)
            seen.add(img)
            
    for img in custom:
        if img not in seen:
            combined.append(img)
            seen.add(img)
            
    return combined

def add_custom_image(image: str):
    """Adds a new image to the custom list if not already present."""
    image = image.strip()
    if not image: 
        return
        
    config = load_config()
    custom = config.get("custom_images", [])
    
    if image not in custom and image not in DEFAULT_IMAGES:
        custom.append(image)
        config["custom_images"] = custom
        save_config(config)
