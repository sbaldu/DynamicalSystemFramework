
FROM almalinux:9

COPY ./ /app

RUN dnf install -y gcc-c++ \
				   clang-tools-extra \
				   make \
				   cmake \
				   gdb \
				   valgrind \
				   git \
				   python3-pip \
				   vim

WORKDIR /app
