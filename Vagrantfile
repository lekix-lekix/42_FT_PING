Vagrant.configure("2") do |config|
  config.vm.box = "generic/debian12"
  config.vm.synced_folder "./", "/home/vagrant/ft_ping"  

  config.vm.define "ft_ping_vm" do |ft_ping_vm|
    ft_ping_vm.vm.provider "virtualbox"
    ft_ping_vm.vm.provision "shell", privileged: false, inline: <<-SHELL
      sudo apt-get update && sudo apt-get install -y inetutils-ping valgrind tcpdump --fix-missing
    SHELL
  end
end