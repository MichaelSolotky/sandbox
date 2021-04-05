# add specific cuda version
FROM nvidia/cuda:10.1-cudnn7-runtime-ubuntu18.04

# update and upgrade apt
RUN apt-get update -y --fix-missing && apt-get upgrade -y
# RUN DEBIAN_FRONTEND=noninteractive apt-get install -y --no-install-recommends apt-utils

# configure locales
RUN apt-get install -y locales locales-all
ENV LC_ALL en_US.UTF-8
ENV LANG en_US.UTF-8
ENV LANGUAGE en_US.UTF-8

# install basic bash comands from apt
RUN apt-get install -yqq build-essential
RUN apt-get install -yqq checkinstall
RUN apt-get install -yqq cmake gcc g++
RUN apt-get install -yqq curl wget
RUN apt-get install -yqq docker
RUN apt-get install -yqq ffmpeg
RUN apt-get install -yqq git
RUN apt-get install -yqq graphviz
RUN apt-get install -yqq htop
RUN apt-get install -yqq libboost-all-dev

# Needed in order to install python
# RUN apt-get install -yqq libpython3.5-dev libpython3.6-dev libpython3.7-dev
RUN apt-get install -yqq libpython3.6-dev libpython3.7-dev
# RUN apt-get install -yqq libreadline-gplv2-dev libncursesw5-dev libssl-dev libsqlite3-dev tk-dev \
#                          libgdbm-dev libc6-dev libbz2-dev libffi-dev zlib1g-dev
RUN apt-get install -yqq zlib1g-dev libncurses5-dev libgdbm-dev libnss3-dev libssl-dev libreadline-dev \
                         libffi-dev libsqlite3-dev libbz2-dev

RUN apt-get install -yqq liblzma-dev
RUN apt-get install -yqq python-dev python-opengl python3-dev python3-opengl python3-pip
RUN apt-get install -yqq software-properties-common
RUN apt-get install -yqq vim zsh
RUN apt-get install -yqq xvfb
RUN apt-get install -yqq zip unzip


RUN add-apt-repository ppa:deadsnakes/ppa
RUN apt-get update
RUN apt-get install -yqq python3.6
RUN python3.6 -m pip install --upgrade pip
# RUN apt-get install -yqq python3.7 python3.7-distutils
# RUN python3.7 -m pip install --upgrade pip
RUN alias python="python3.6"
RUN alias pip="pip3"

# install pip packages
RUN pip3 install --upgrade pip setuptools wheel
RUN pip3 install albumentations
RUN pip3 install bokeh
RUN pip3 install casadi
RUN pip3 install catboost
RUN pip3 install cvxopt
RUN pip3 install cvxpy
RUN pip3 install cython
RUN pip3 install editdistance
RUN pip3 install efficientnet_pytorch
RUN pip3 install flask
RUN pip3 install gekko
RUN pip3 install gensim
RUN pip3 install gym
RUN pip3 install gym[atari]
RUN pip3 install gym[box2d]
RUN pip3 install hyperopt
RUN pip3 install imgaug
RUN pip3 install ipywidgets
RUN pip3 install jupyter
RUN pip3 install jupyter_contrib_nbextensions
RUN pip3 install keras
RUN pip3 install --upgrade https://github.com/Lasagne/Lasagne/archive/master.zip
RUN pip3 install librosa
RUN pip3 install lightgbm
RUN pip3 install loguru
RUN pip3 install lmdb
RUN pip3 install matplotlib
RUN pip3 install mpld3
RUN pip3 install moviepy
RUN pip3 install networkx
RUN pip3 install numba
RUN pip3 install numpy
RUN pip3 install nltk
RUN pip3 install nose
RUN pip3 install pandas
RUN pip3 install Pillow
RUN pip3 install protobuf
RUN pip3 install psutil
RUN pip3 install pycocotools
RUN pip3 install pygame
RUN pip3 install pyLDAvis
RUN pip3 install pylint
RUN pip3 install pytest
RUN pip3 install pytorch-ignite
RUN pip3 install pytorch_lightning
RUN pip3 install PyWavelets
RUN pip3 install pyyaml
RUN pip3 install regex
RUN pip3 install scikit-image
RUN pip3 install scikit-learn
RUN pip3 install scipy
RUN pip3 install seaborn
RUN pip3 install sklearn
RUN pip3 install sphinx
RUN pip3 install sphinx_rtd_theme
RUN pip3 install statsmodels
RUN pip3 install stop_words
RUN pip3 install submitit
RUN pip3 install subword-nmt
RUN pip3 install tensorflow-gpu==2.2.0
RUN pip3 install timm
RUN pip3 install theano
RUN pip3 install torch
RUN pip3 install torchvision
RUN pip3 install torchsummary
RUN pip3 install tqdm
RUN pip3 install wheel
RUN pip3 install xlrd

# configure a working directory
RUN mkdir /root/data
WORKDIR /root/data

# customize shell
RUN git clone https://github.com/ohmyzsh/ohmyzsh.git ../.oh-my-zsh
RUN git clone https://github.com/MichaelSolotky/sandbox.git
RUN cp sandbox/Configs/zshrc ../.zshrc
RUN cp sandbox/Configs/vimrc ../.vimrc

# configure jupyter
RUN jupyter nbextension enable --py --sys-prefix widgetsnbextension
RUN jupyter contrib nbextension install --user
RUN jupyter nbextension enable codefolding/main
RUN echo "c.NotebookApp.ip = '*'" >> /root/.jupyter/jupyter_notebook_config.py
RUN echo "c.NotebookApp.port = 8888" >> /root/.jupyter/jupyter_notebook_config.py
RUN echo "c.NotebookApp.token = ''" >> /root/.jupyter/jupyter_notebook_config.py
CMD ["jupyter", "notebook", "--no-browser", "--allow-root"] 
EXPOSE 8888
