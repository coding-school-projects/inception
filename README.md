<h1 align="center">
  <img src="https://github.com/senthilpoo10/badges/blob/main/badges/inceptione.png" width="200"/>
</h1>

<p align="center">
  <b><i>Inception: Docker Infrastructure Project</i></b><br>
</p>

<p align="center">
  <img alt="score" src="https://img.shields.io/badge/score-100%2F100-brightgreen" />
  <img alt="solo" src="https://img.shields.io/badge/solo-yellow" />
  <img alt="time spent" src="https://img.shields.io/badge/time%20spent-80%20hours-blue" />
  <img alt="XP earned" src="https://img.shields.io/badge/XP%20earned-852-orange" />
<p align="center">
  <img alt="GitHub code size in bytes" src="https://img.shields.io/github/languages/code-size/coding-school-projects/inception?color=lightblue" />
  <img alt="GitHub top language" src="https://img.shields.io/github/languages/top/coding-school-projects/inception?color=blue" />
  <img alt="GitHub last commit" src="https://img.shields.io/github/last-commit/coding-school-projects/inception?color=green" />
</p>

## 📚 About The Project

This project creates a small infrastructure using Docker containers with NGINX, WordPress, and MariaDB services. It demonstrates containerization, networking, and proper system administration practices.

**Key Components:**
- NGINX container with TLS encryption
- WordPress + php-fpm container
- MariaDB container with persistent volumes
- Custom Docker network
- Environment variable configuration
- Automatic restart policies

## 🏁 Getting Started

### 📊 Flowchart

![Flow Chart](https://github.com/coding-school-projects/inception/blob/main/flowchart.png)

### 📝 Required Configuration
| File | Purpose |
|------|---------|
| `srcs/.env` | Stores all sensitive credentials |
| `srcs/requirements/nginx/conf/nginx.conf` | NGINX configuration |
| `srcs/requirements/mariadb/conf/my.cnf` | MariaDB configuration |
| `srcs/docker-compose.yml` | Container orchestration |

## 🧠 Technical Implementation

### Core Architecture
| Service | Implementation Details |
|---------|------------------------|
| **NGINX** | TLS 1.3 only, reverse proxy configuration |
| **WordPress** | php-fpm 8.2, connected to MariaDB |
| **MariaDB** | Persistent volume, two users configured |
| **Network** | Custom bridge network for container communication |
| **Volumes** | Two persistent volumes for DB and website files |

### Security Features
| Feature | Implementation |
|---------|---------------|
| TLS Encryption | Let's Encrypt certificates |
| Database Users | Custom admin username (not "admin") |
| Environment Variables | No hardcoded credentials |
| Container Isolation | Dedicated containers for each service |

### 🛠️ Installation & Usage

1. Clone the repository:
```bash
git clone https://github.com/coding-school-projects/inception.git
cd inception
```

2. Configure environment variables:
```bash
cp srcs/.env.example srcs/.env
nano srcs/.env  # Edit with your credentials
```

3. Build and launch the infrastructure:
```bash
make
```

4. Access your WordPress site at:
```
https://yourlogin.42.fr
```

## 🧪 Testing Protocol

1. Verify containers are running:
```bash
docker ps -a
```

2. Check NGINX TLS configuration:
```bash
openssl s_client -connect yourlogin.42.fr:443 -tls1_3
```

3. Test WordPress installation:
- Access https://yourlogin.42.fr
- Complete WordPress setup
- Verify posts persist after container restart

4. Test database persistence:
```bash
docker exec -it mariadb mysql -u root -p
SHOW DATABASES;
```

## 📝 Evaluation Criteria

1. **Mandatory Requirements**:
   - All services in separate containers
   - Proper volume usage
   - TLS encryption
   - No infinite loop hacks
   - Correct restart policies

2. **Bonus Requirements**:
   - Redis cache for WordPress
   - FTP server for WordPress files
   - Static website (non-PHP)
   - Adminer installation
   - Additional useful service

### 🧑‍💻 Peer Evaluations (3/3)

> **Peer 1**: "Poonkodi did a great job explaining this project to me. had horrible flashbacks to born to be root. Some of the instructions were not very clear, but she explained them to me. Best of luck in the final evaluation."

> **Peer 2**: "Wow, great job, Poonkodi! We went through her inception and tested it with the evaluation sheet, and everything worked perfectly. She understood the project's concepts and all the details, and explained them very well! I'm now working on this project, so it was a really big help for me to understand it better. Thanks for all the tips, and have a nice long weekend :)"

> **Peer 3**: "Poonkodi has a deep understanding on this subject. Since it was totally new for me, the evaluation was a long session, but very insightful. She ensure that I truly understand her explanation and could answer all my questions. Everything was tested following the evaluation guideline, and no problem was found. Thanks for this opportunity to get in touch with inception project for the first time :)"

## ⚠️ Important Notes

- All credentials must be stored in `srcs/.env` (not committed to Git)
- The domain must be configured in your hosts file:
```bash
echo "127.0.0.1 yourlogin.42.fr" | sudo tee -a /etc/hosts
```
- Virtual Machine is required for this project
- Alpine/Debian must be used as base images


# 📚 Installation steps for Alpine, VM and SSH:

# **Alpine Linux Virtual Machine Setup Guide**

## **Table of Contents**
1. [Downloading Alpine ISO](#1-downloading-alpine-iso)
2. [Creating the Virtual Machine](#2-creating-the-virtual-machine)
3. [Installing Alpine Linux](#3-installing-alpine-linux)
4. [Initial System Configuration](#4-initial-system-configuration)
5. [Setting Up Sudo Privileges](#5-setting-up-sudo-privileges)
6. [Configuring SSH Access](#6-configuring-ssh-access)
7. [Installing a Graphical Interface](#7-installing-a-graphical-interface)
8. [Installing Docker](#8-installing-docker)
9. [Setting Up Project Environment](#9-setting-up-project-environment)

---

## **1. Downloading Alpine ISO**
1. Visit the [Alpine Linux Downloads page](https://alpinelinux.org/downloads/)
2. Under "Older releases", click the provided link
3. Navigate to version `v3.20/`
4. Enter the `releases/` directory
5. Select the `x86_64/` architecture folder
6. Download `alpine-virt-3.20.0-x86_64.iso`

The ISO file will be saved to your default downloads directory.

---

## **2. Creating the Virtual Machine**
1. Open VirtualBox and click "New"
2. Configure VM settings:
   - Name: `Alpine Linux`
   - Type: Linux
   - Version: Other Linux (64-bit)
   - Memory: 2048MB (recommended)
   - Hard disk: Create virtual hard disk now (VDI, dynamically allocated, 10GB)
3. After creation, go to VM Settings:
   - System → Processor: 2 CPUs
   - Display → Video Memory: 128MB
   - Storage → Empty IDE: Attach Alpine ISO
   - Network → Bridged Adapter (or NAT with port forwarding)

---

## **3. Installing Alpine Linux**
1. Start the VM and login as `root` (no password)
2. Run the installation:
   ```sh
   setup-alpine
   ```
3. Follow the interactive setup:
   - Keyboard: `us` (US QWERTY)
   - Hostname: `pchennia.42.fr` (or custom name)
   - Network: DHCP for automatic configuration
   - Root password: Set to `secret`
   - Timezone: `Europe/Helsinki`
   - Mirror: Default (press Enter)
   - User account:
     - Username: `pchennia` (or `testuser`)
     - Password: `secret`
   - Disk: `sda` with `sys` installation type
4. Confirm disk erasure with `y`

---

## **4. Initial System Configuration**
1. Remove installation media from VirtualBox settings
2. Reboot the system:
   ```sh
   reboot
   ```
3. Login with your new user credentials

---

## **5. Setting Up Sudo Privileges**
1. Switch to root:
   ```sh
   su -
   ```
2. Enable community repository:
   ```sh
   vi /etc/apk/repositories
   ```
   Uncomment the community repo line and save.

3. Update and install sudo:
   ```sh
   apk update
   apk add sudo
   ```

4. Configure sudo access:
   ```sh
   visudo
   ```
   Uncomment `%sudo ALL=(ALL:ALL) ALL` and save.

5. Create sudo group and add user:
   ```sh
   addgroup sudo
   adduser pchennia sudo
   ```

---

## **6. Configuring SSH Access**
1. Edit SSH configuration:
   ```sh
   sudo vi /etc/ssh/sshd_config
   ```
   - Change port to `4242`
   - Set `PermitRootLogin no`

2. Restart SSH service:
   ```sh
   sudo rc-service sshd restart
   ```

3. Set up port forwarding in VirtualBox:
   - Host Port: 4242
   - Guest Port: 4242

4. Test connection from host:
   ```sh
   ssh pchennia@localhost -p 4242
   ```

---

## **7. Installing a Graphical Interface**
1. Install XFCE desktop environment:
   ```sh
   sudo apk add xorg-server xfce4 xfce4-terminal lightdm
   ```

2. Configure display manager:
   ```sh
   sudo setup-xorg-base
   sudo rc-update add lightdm
   ```

3. Reboot to start GUI:
   ```sh
   sudo reboot
   ```

---

## **8. Installing Docker**
1. Add Docker repository:
   ```sh
   sudo vi /etc/apk/repositories
   ```
   Uncomment community repository.

2. Install Docker components:
   ```sh
   sudo apk add docker docker-compose
   sudo rc-update add docker boot
   sudo service docker start
   ```

3. Add user to docker group:
   ```sh
   sudo addgroup pchennia docker
   ```

---

## **9. Setting Up Project Environment**
1. Create project directory:
   ```sh
   mkdir ~/inception && cd ~/inception
   ```

2. Set proper permissions:
   ```sh
   sudo chown -R pchennia:pchennia .
   sudo chmod 775 .
   ```

3. Install additional tools:
   ```sh
   sudo apk add make git curl
   ```

4. Your development environment is now ready!

---

## **Final Notes**
- Always use `sudo` for administrative commands
- The system is configured with enhanced security:
  - Non-root user with sudo privileges
  - Custom SSH port with root login disabled
  - Proper file permissions

For maintenance:
- Regular updates: `sudo apk update && sudo apk upgrade`
- Backup important files regularly

This setup provides a secure, lightweight Alpine Linux environment suitable for development and containerized applications.
