# 基于 Ubuntu 20.04 构建
FROM ubuntu:20.04

# 关闭系统交互提示
ENV DEBIAN_FRONTEND=noninteractive

# 替换为阿里云镜像源（核心修改！解决下载慢/超时）
RUN sed -i s@/archive.ubuntu.com/@/mirrors.aliyun.com/@g /etc/apt/sources.list && \
    apt update && apt install -y \
  g++ \
  libodb-dev=2.4.0-1build1 \
  libodb-mysql-dev=2.4.0-1build1 \
  libodb-boost-dev=2.4.0-1build1 \
  libmysqlclient-dev=8.0.25-0ubuntu0.20.04.1 \
  libboost-all-dev \
  make

# 设置工作目录
WORKDIR /app

# 复制项目文件
COPY . /app

# 编译运行
CMD make && ./main