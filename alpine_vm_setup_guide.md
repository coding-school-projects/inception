
# 🧊 Alpine Linux VM Setup, SSH Configuration, GUI, Docker, and Project Prep Guide

---

## 1. Download Alpine ISO
1. Go to [https://alpinelinux.org/downloads/](https://alpinelinux.org/downloads/)
2. Click the **"here"** link under **"Older releases are found here."**
3. Click on `v3.20/`
4. Click `releases/`
5. Click `x86_64/`
6. Download `alpine-virt-3.20.0-x86_64.iso`
7. ISO will be available in your **Downloads** folder.

---

## 2. Create and Install Alpine VM (VirtualBox)
1. Boot VM using the ISO.
2. Login: `root`
3. Run: `setup-alpine`

### Setup Wizard Prompts:
- Keyboard layout: `us`
- Variant: `us`
- Hostname: `pchennia.42.fr` (or `testuser.42.fr` at home)
- Network: 
  - Interface: `eth0` (just press Enter)
  - DHCP: Press Enter
  - Manual config: Press Enter (default `n`)
- Root Password: `secret`
- Time Zone: 
  - Region: `Europe`
  - City: `Helsinki`
- Proxy: Press Enter (default `none`)
- NTP Client: Press Enter (default `chrony`)
- Mirror: Press Enter (default `1`)
- Create User: 
  - Username: `pchennia` (or `testuser`)
  - Full name: Press Enter
  - Password: `secret`
  - SSH key: Press Enter
  - SSH server: Press Enter
- Disk:
  - Disk: `sda`
  - Mode: `sys`
  - Confirm format: `y`

4. After installation:
   - Go to **VirtualBox → Settings → Storage**
   - Right-click ISO → **Remove Disk from Virtual Drive**
   - Click **OK**

5. Back in the VM terminal:  
   - Run `reboot`

---

## 3. Post-Install Setup and Sudo Configuration
1. Login as `testuser`
2. Check for sudo group:
   ```bash
   sudo ls
   ```
3. If not available:
   ```bash
   addgroup sudo
   getent group sudo
   ```
4. Confirm you're root: `whoami` → should return `root`
5. Edit repositories:
   ```bash
   vi /etc/apk/repositories
   ```
   - Uncomment:
     ```
     http://alpine.mirror.wearetriple.com/v3.18/community
     ```
   - Save and exit (`Esc`, `:wq`)
6. Update and install `sudo`:
   ```bash
   apk update
   apk add sudo
   sudo visudo
   ```
   - Uncomment:
     ```
     %sudo ALL=(ALL:ALL) ALL
     ```
7. Add user to sudo:
   ```bash
   adduser testuser sudo
   ```

---

## 4. SSH Configuration
1. Switch to user:
   ```bash
   su - testuser
   sudo apk update
   ```
2. Edit SSHD config:
   ```bash
   sudo vi /etc/ssh/sshd_config
   ```
   - Set:
     ```
     Port 4242
     PermitRootLogin no
     ```
3. (Optional) Edit client config:
   ```bash
   sudo vi /etc/ssh/ssh_config
   ```
   - Set:
     ```
     Port 4242
     ```
4. Restart SSH service:
   ```bash
   sudo rc-service sshd restart
   ```
5. Confirm SSH port:
   ```bash
   netstat -tuln | grep 4242
   ```
6. Configure VirtualBox Port Forwarding:
   - Go to **Settings → Network → Advanced → Port Forwarding**
   - Add rule:  
     - Host Port: `4242`  
     - Guest Port: `4242`

---

## 5. SSH Access From Host Terminal
1. From host:
   ```bash
   ssh localhost -p 4242
   ```
2. If warning/error:
   ```bash
   nano ~/.ssh/known_hosts
   ```
   - Clear file contents and save.
3. Try SSH again:
   ```bash
   ssh localhost -p 4242
   ```
   - Password: `secret`

---

## 6. Install Graphical User Interface
```bash
sudo apk add xorg-server xfce4 xfce4-terminal lightdm lightdm-gtk-greeter xf86-input-libinput elogind
sudo setup-devd udev
sudo rc-update add elogind
sudo rc-update add lightdm
sudo reboot
```
- On reboot, GUI should be visible.

---

## 7. Docker and Docker Compose Setup
1. SSH into VM:
   ```bash
   ssh localhost -p 4242
   ```
2. Edit repositories:
   ```bash
   sudo vi /etc/apk/repositories
   ```
   - Uncomment:
     ```
     /media/cdrom/apks
     ```
3. Install Docker:
   ```bash
   sudo apk add docker docker-compose
   sudo apk add --update docker openrc
   sudo rc-update add docker boot
   sudo service docker start
   ```
4. Add user to Docker group:
   ```bash
   sudo addgroup testuser docker
   ```
5. Install Docker CLI Compose:
   ```bash
   sudo apk add docker-cli-compose
   ```

---

## 8. Additional Tools
- Install `make`:
  ```bash
  sudo apk add --no-cache make
  ```
- Install `firefox`:
  ```bash
  sudo apk add firefox-esr
  ```

---

## 9. Project Setup via SCP
1. Create `inception` folder on your host machine.
2. Use `scp` to copy project files to VM:
   ```bash
   scp -P 4242 -r inception testuser@localhost:~
   ```
3. Change ownership and permissions:
   ```bash
   sudo chown -R testuser:testuser Makefile
   sudo chown -R testuser:testuser srcs
   sudo chown -R testuser:testuser .
   sudo chown -R testuser:testuser ..
   
   sudo chmod 775 .
   sudo chmod 1775 ..
   sudo chmod 1777 ..
   sudo chmod 664 Makefile
   sudo chmod 775 srcs
   ```
4. Confirm structure:
   ```bash
   ls -alR
   ```

---

## ✅ Ready for Dockerfile and Project Code
You can now start writing your Dockerfile and setup project code within the `inception` folder inside your VM or local system and sync using `scp`.
```bash
scp -P 2288 -r /home/pchennia/Inception localhost:/home/pchennia
```
The above is line just copy the folder from local system to VM machine using scp command

```bash
ssh localhost -p 2288
```
The above line is connect to the VM machine from local terminal. So We can local terminal as the VM's terminal.
