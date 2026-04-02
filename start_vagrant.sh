#!/bin/bash -x

git clone git@vogsphere.42paris.fr:vogsphere/intra-uuid-252f9376-6ffd-4387-aa93-065d98ddfea9-7316507-kipouliq ft_ping

export VAGRANT_HOME=/goinfre/$USER/.vagrant.d
vagrant up