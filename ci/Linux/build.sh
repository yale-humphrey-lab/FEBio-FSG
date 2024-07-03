#! /bin/bash
# Uncomment next line if not global on target machine
set -e
#git config --global --add safe.directory /FEBio
source "/opt/intel/oneapi/setvars.sh" --force
cmake . -B cmbuild -LA \
	-DUSE_MMG=ON \
	-DUSE_LEVMAR=ON \
	-DUSE_HYPRE=ON
pushd cmbuild
make -j $(nproc)
popd
