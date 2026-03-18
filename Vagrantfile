Vagrant.configure("2") do |config|
  config.vm.box = "generic/debian12"
  config.vm.synced_folder "~/.ssh", "/home/vagrant/.ssh_host"  # removed type constraint

  config.vm.define "ft_ping_vm" do |ft_ping_vm|
    ft_ping_vm.vm.provider "virtualbox"
    ft_ping_vm.vm.provision "shell", privileged: false, inline: <<-SHELL

      sudo apt-get update && sudo apt-get install -y inetutils-ping valgrind --fix-missing
      
      mkdir -p ~/.ssh
      chmod 700 ~/.ssh

      # Copy whichever key exists
      cp /home/vagrant/.ssh_host/id_ed25519 ~/.ssh/id_ed25519 2>/dev/null || \
      cp /home/vagrant/.ssh_host/id_rsa ~/.ssh/id_rsa 2>/dev/null

      chmod 600 ~/.ssh/id_ed25519 ~/.ssh/id_rsa 2>/dev/null

      # Tell SSH explicitly which key to use for GitHub
      cat > ~/.ssh/config <<EOF
Host github.com
  HostName github.com
  User git
  IdentityFile ~/.ssh/id_rsa
  StrictHostKeyChecking no
EOF
      chmod 600 ~/.ssh/config

      ssh-keyscan github.com >> ~/.ssh/known_hosts 2>/dev/null

      git clone git@github.com:lekix-lekix/42_FT_PING.git ft_ping
    SHELL
  end
end