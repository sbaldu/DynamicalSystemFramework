
FROM almalinux:9

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
