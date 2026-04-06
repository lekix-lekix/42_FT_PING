# 📡 ft_ping — Custom Ping Implementation 

## 🧠 Overview

`ft_ping` is a custom implementation of the `ping` command, built using **raw sockets**.  
This project allows sending ICMP Echo requests to a target and measuring response times while working directly with low-level network layers.

---

## ⚙️ Features

- Send ICMP Echo Request packets
- Receive and parse ICMP Echo Reply packets
- Handle network errors
- Display statistics (RTT, packet loss, etc.)
- Uses **raw sockets** (requires root privileges)

---

## 🧩 Supported Options

| Option        | Description |
|--------------|------------|
| `--ttl`      | Set the Time To Live for packets |
| `--pattern`  | Define a custom payload pattern |
| `--count`    | Number of requests to send |
| `--timeout`  | Maximum wait time for a reply |
| `--interval` | Delay between each request |
| `--verbose`  | Enable verbose mode |

---

## 🛠️ Requirements

- 🐧 Linux (**Ubuntu / Debian** recommended)
- 📦 `apt`
- 📦 Vagrant
- 📦 VirtualBox (or another compatible provider)

---

## 🚀 Installation & Usage

### 1. Clone the repository

    git clone https://github.com/lekix-lekix/ft_ping.git
    cd ft_ping

### 2. Start the Vagrant environment

    ./start_vagrant

### 3. Connect to the virtual machine

    vagrant ssh

### 4. Build the project

    cd ft_ping && make

### 5. Run ft_ping

    ft_ping <hostname>

Example:

    ft_ping google.com

---

## 🧪 Example with options

    ft_ping google.com --count 5 --interval 1 --ttl 64 --verbose

---

## 🔐 Permissions

Using **raw sockets** requires administrative privileges:

    sudo ./ft_ping <hostname>

---

## 🏗️ Architecture

- Uses `AF_INET` and `SOCK_RAW`
- Manual ICMP packet construction
- Checksum calculation
- RTT (round-trip time) handling
- Network response parsing

---

## 📊 Expected Output

- Output similar to `ping`
- Response time in milliseconds
- Final statistics:
  - packets sent / received
  - packet loss (%)
  - min / avg / max RTT / standard deviation

---

## 📌 Notes

- Educational project to understand ICMP and low-level networking
- Requires `sudo` in most cases
- Tested on Ubuntu / Debian

---

## 👨‍💻 Author

Project developed as part of learning network systems and low-level C programming at 42 Paris school.
