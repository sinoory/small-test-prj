# 使用DaoCloud的Ubuntu镜像
#FROM daocloud.io/library/ubuntu:14.04
FROM ubuntu:14.04
# 设置镜像作者
MAINTAINER Fundebug <help@fundebug.com>
# 设置时区
RUN sudo sh -c "echo 'Asia/Shanghai' > /etc/timezone" && \
    sudo dpkg-reconfigure -f noninteractive tzdata
# 安装node v6.10.1
RUN sudo apt-get update && sudo apt-get install -y wget
# 使用淘宝镜像安装Node.js v6.10.1
RUN wget https://nodejs.org/download/release/v6.10.1/node-v6.10.1-linux-x64.tar.gz && \
    tar -C /usr/local --strip-components 1 -xzf node-v6.10.1-linux-x64.tar.gz && \
    rm node-v6.10.1-linux-x64.tar.gz 
WORKDIR /app
# 安装npm模块
ADD package.json /app/package.json
# 使用淘宝的npm镜像
RUN npm install 
# 添加源代码
ADD . /app

ENV PORT=80

# 运行app.js
CMD ["npm", "start"]
