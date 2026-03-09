Vagrant.configure("2") do |config|
  config.vm.box = "generic/debian12"

  config.vm.define "ft_ping_vm" do |ft_ping_vm|
    ft_ping_vm.vm.provider "virtualbox"

    server.vm.provision "shell", inline: <<-SHELL
        sudo apt update
        sudo apt-install inetutils-ping -y 
    SHELL
  end
end
