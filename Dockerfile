FROM archlinux:base-devel

# Perform initial system update
RUN pacman -Syu --noconfirm

# Install base requirements for development
RUN pacman -S --noconfirm arm-none-eabi-gcc arm-none-eabi-newlib cmake git

# Install extra packages
RUN pacman -S --noconfirm wget   # Required for pulling JLink GDB server
RUN pacman -S --noconfirm neovim # Include neovim for editing

# Pull JLink GDB server package 
RUN wget --post-data "accept_license_agreement=accepted" https://www.segger.com/downloads/jlink/JLink_Linux_V812b_x86_64.tgz
# Unpack and remove tarball
RUN tar -xzf JLink_Linux_V812b_x86_64.tgz && rm -r JLink_Linux_V812b_x86_64.tgz
# Add to path
ENV PATH="/JLink_Linux_V812b_x86_64:${PATH}"

# Copy over repo files
COPY . $HOME/Australis-Avionics-firmware/
