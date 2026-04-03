# 📡 ft_ping — Custom Ping Implementation (Raw Sockets)

## 🧠 Overview

`ft_ping` est une implémentation personnalisée de la commande `ping`, développée en utilisant des **raw sockets**.  
Ce projet permet d’envoyer des requêtes ICMP Echo vers une cible et de mesurer les temps de réponse, tout en manipulant directement les couches basses du réseau.

---

## ⚙️ Fonctionnalités

- Envoi de paquets ICMP Echo Request
- Réception et analyse des ICMP Echo Reply
- Gestion des erreurs réseau
- Affichage des statistiques (RTT, pertes, etc.)
- Utilisation de **raw sockets** (nécessite privilèges root)

---

## 🧩 Options supportées

| Option        | Description |
|--------------|------------|
| `--ttl`      | Définit le Time To Live des paquets |
| `--pattern`  | Définit un motif personnalisé dans les données envoyées |
| `--count`    | Nombre de requêtes à envoyer |
| `--timeout`  | Temps maximum d’attente pour une réponse |
| `--interval` | Intervalle entre chaque requête |
| `--verbose`  | Active un mode verbeux |

---

## 🛠️ Prérequis

- 🐧 Linux (**Ubuntu / Debian** recommandé)
- 📦 `apt`
- 📦 Vagrant
- 📦 VirtualBox (ou autre provider compatible)

---

## 🚀 Installation & Exécution

### 1. Cloner le repository

    git clone https://github.com/your-username/ft_ping.git
    cd ft_ping

### 2. Lancer l’environnement Vagrant

    ./start_vagrant

### 3. Se connecter à la machine virtuelle

    vagrant ssh

### 4. Compiler le projet (si nécessaire)

    make

### 5. Exécuter ft_ping

    sudo ./ft_ping <hostname>

Exemple :

    sudo ./ft_ping google.com

---

## 🧪 Exemple avec options

    sudo ./ft_ping google.com --count 5 --interval 1 --ttl 64 --verbose

---

## 🔐 Permissions

L’utilisation de **raw sockets** nécessite des privilèges administrateur :

    sudo ./ft_ping <hostname>

---

## 🏗️ Architecture

- Utilisation de `AF_INET` et `SOCK_RAW`
- Construction manuelle des paquets ICMP
- Calcul du checksum
- Gestion du timing (RTT)
- Parsing des réponses réseau

---

## 📊 Résultat attendu

- Affichage similaire à `ping`
- Temps de réponse en ms
- Statistiques finales :
  - paquets envoyés / reçus
  - perte (%)
  - min / avg / max RTT

---

## 📌 Notes

- Projet pédagogique pour comprendre ICMP et les sockets bas niveau
- Nécessite souvent `sudo`
- Testé sur Ubuntu / Debian

---

## 👨‍💻 Auteur

Projet réalisé dans le cadre de l’apprentissage des systèmes réseaux et de la programmation bas niveau en C.