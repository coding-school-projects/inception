<h1 align="center">
  <img src="https://github.com/senthilpoo10/badges/blob/main/badges/dockerm.png" width="200"/>
</h1>

<p align="center">
  <b><i>Inception: Docker Infrastructure Project</i></b><br>
  <i>"WE NEED TO GO DEEPER"</i>
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
```
