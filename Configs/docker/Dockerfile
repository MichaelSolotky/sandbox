# add a specific cuda version
FROM nvidia/cuda:10.0-cudnn7-runtime-ubuntu16.04

# install basic bash comands from apt
RUN apt-get update
RUN apt-get install -yqq python3-pip vim wget

# install pip packages
RUN pip3 install --upgrade pip
RUN pip3 install bokeh
# RUN pip3 install box2d # fails
# RUN pip3 install catboost # retries
RUN pip3 install cvxopt
RUN pip3 install cvxpy
RUN pip3 install cython
# RUN pip3 install dionysus # fails
RUN pip3 install flask
RUN pip3 install gensim
RUN pip3 install gym
RUN pip3 install imgaug
RUN pip3 install ipywidgets
RUN pip3 install jupyter jupyter_contrib_nbextensions
RUN pip3 install keras
RUN pip3 install librosa
RUN pip3 install lightgbm
RUN pip3 install matplotlib
RUN pip3 install moviepy
RUN pip3 install networkx
RUN pip3 install numpy
RUN pip3 install nltk
RUN pip3 install pandas
RUN pip3 install Pillow
RUN pip3 install protobuf
RUN pip3 install pyLDAvis
RUN pip3 install pylint
RUN pip3 install pytest
RUN pip3 install PyWavelets
RUN pip3 install regex
RUN pip3 install scikit-image scikit-learn scikit-tda
RUN pip3 install scipy
RUN pip3 install seaborn
RUN pip3 install sklearn
RUN pip3 install sphinx
RUN pip3 install sphinx_rtd_theme
RUN pip3 install statsmodels
# RUN pip3 install stop_word # fails
# RUN pip3 install tensorflow-gpu=2.0.0 tensorboard # retries
RUN pip3 install torch torchvision
RUN pip3 install tqdm
RUN pip3 install wheel
RUN pip3 install xlrd

# configure jupyter
RUN jupyter nbextension enable --py --sys-prefix widgetsnbextension
RUN jupyter contrib nbextension install --user
RUN jupyter nbextension enable codefolding/main
RUN echo "c.NotebookApp.ip = '*'" >> /root/.jupyter/jupyter_notebook_config.py
RUN echo "c.NotebookApp.port = 8888" >> /root/.jupyter/jupyter_notebook_config.py
RUN echo "c.NotebookApp.token = ''" >> /root/.jupyter/jupyter_notebook_config.py
CMD ["jupyter", "notebook", "--no-browser", "--allow-root"]
EXPOSE 8888

# configure a working directory
RUN mkdir /root/data
WORKDIR /root/data
