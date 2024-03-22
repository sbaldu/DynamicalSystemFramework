
FROM almalinux:8

COPY ./ /app

RUN dnf install -y gcc-c++ \
				   clang-tools-extra \
				   make \
				   cmake \
				   gdb \
				   valgrind \
				   python3-pip \
				   vim

WORKDIR /app
