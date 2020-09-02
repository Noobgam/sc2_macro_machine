sudo docker build -t sc2-macro-machine-autotest .
sudo docker tag sc2-macro-machine-autotest:latest 928383298076.dkr.ecr.eu-central-1.amazonaws.com/sc2-macro-machine-autotest:latest
sudo docker push 928383298076.dkr.ecr.eu-central-1.amazonaws.com/sc2-macro-machine-autotest:latest
