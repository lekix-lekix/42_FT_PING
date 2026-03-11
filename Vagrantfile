Vagrant.configure("2") do |config|
  config.vm.box = "generic/debian12"

  config.vm.synced_folder "~/.ssh", "/home/vagrant/.ssh_host", type: "virtualbox"

  config.vm.define "ft_ping_vm" do |ft_ping_vm|
    ft_ping_vm.vm.provider "virtualbox"

    ft_ping_vm.vm.provision "shell", inline: <<-SHELL
        sudo apt update
        sudo apt-get install inetutils-ping -y

        # Copie les clés SSH de l'hôte
        cp /home/vagrant/.ssh_host/id_rsa ~/.ssh/id_rsa 2>/dev/null || \
        cp /home/vagrant/.ssh_host/id_ed25519 ~/.ssh/id_ed25519 2>/dev/null

        chmod 600 ~/.ssh/id_*

        # Config Git
        # git config --global user.email "ton@email.com"
        # git config --global user.name "Ton Nom"

        # Accepte github.com sans prompt
        ssh-keyscan github.com >> ~/.ssh/known_hosts 2>/dev/null

        rm -rf ft_ping
        git clone git@github.com:lekix-lekix/42_FT_PING.git ft_ping
        sudo chown vagrant:vagrant -R ./ft_ping && sudo chown vagrant:vagrant ./ft_ping
    SHELL
  end
end
