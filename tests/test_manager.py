import unittest
from unittest.mock import patch, MagicMock
import json
import subprocess
from core.manager import DistroboxManager

class TestDistroboxManager(unittest.TestCase):
    def setUp(self):
        self.manager = DistroboxManager()

    @patch('shutil.which')
    def test_is_distrobox_installed(self, mock_which):
        # Test installed
        mock_which.return_value = "/usr/bin/distrobox"
        self.assertTrue(self.manager.is_distrobox_installed())
        
        # Test not installed
        mock_which.return_value = None
        self.assertFalse(self.manager.is_distrobox_installed())

    @patch('subprocess.run')
    @patch('shutil.which')
    def test_get_containers_json(self, mock_which, mock_run):
        mock_which.return_value = "/usr/bin/distrobox"
        
        # Mock successful JSON output
        mock_output = [
            {"name": "test-box", "status": "up", "image": "fedora:latest"},
            {"name": "another-box", "status": "exited", "image": "ubuntu:22.04"}
        ]
        mock_run.return_value = MagicMock(stdout=json.dumps(mock_output), returncode=0)
        
        containers = self.manager.get_containers()
        self.assertEqual(len(containers), 2)
        self.assertEqual(containers[0]['name'], "test-box")
        self.assertEqual(containers[1]['image'], "ubuntu:22.04")

    @patch('subprocess.run')
    @patch('shutil.which')
    def test_get_containers_fallback_text(self, mock_which, mock_run):
        mock_which.return_value = "/usr/bin/distrobox"
        
        # Make the first call (JSON) fail, second call (text) succeed
        def side_effect(cmd, **kwargs):
            if "--json" in cmd:
                raise subprocess.CalledProcessError(1, cmd)
            else:
                text_output = (
                    "ID | NAME | STATUS | IMAGE\n"
                    "123 | my-arch | Up 2 hours | archlinux:latest\n"
                )
                return MagicMock(stdout=text_output, returncode=0)
        
        mock_run.side_effect = side_effect
        
        containers = self.manager.get_containers()
        self.assertEqual(len(containers), 1)
        self.assertEqual(containers[0]['name'], "my-arch")
        self.assertEqual(containers[0]['id'], "123")

    def test_get_icon_name(self):
        self.assertEqual(self.manager.get_icon_name("registry.com/ubuntu:20.04"), "ubuntu")
        self.assertEqual(self.manager.get_icon_name("fedora-39"), "fedora")
        self.assertEqual(self.manager.get_icon_name("unknown-distro"), "linux-generic")

    @patch('subprocess.run')
    def test_create_box(self, mock_run):
        mock_run.return_value = MagicMock(returncode=0)
        success = self.manager.create_box("test", "ubuntu")
        self.assertTrue(success)
        mock_run.assert_called_with(["distrobox", "create", "-n", "test", "-i", "ubuntu", "-Y"], check=True)

if __name__ == '__main__':
    unittest.main()
