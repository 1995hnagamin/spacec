FROM akabe/ocaml:alpine3.9_ocaml4.08.1

ENV USERNAME kceage
ENV UID 10000
ENV GID 10000
ENV HOME /home/${USERNAME}

RUN apk add bash bubblewrap m4 git
RUN addgroup -S ${USERNAME}
RUN adduser -S -G ${USERNAME} ${USERNAME}
RUN echo "kceage ALL=(ALL) NOPASSWD:ALL" >> /etc/sudoers
RUN echo 'kceage:kceage' | chpasswd
USER ${USERNAME}

WORKDIR /home/${USERNAME}
ENV OPAMDEPEXTYES 1
RUN opam init --disable-sandboxing
RUN eval $(opam env)
RUN opam install -y ocp-indent omake

USER root
RUN apk add sudo
USER ${USERNAME}
