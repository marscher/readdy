SET(SOURCES_DIR "${READDY_GLOBAL_DIR}/kernels/singlecpu/src")
SET(SINGLECPU_INCLUDE_DIR "${READDY_GLOBAL_DIR}/kernels/singlecpu/include")

# sources
LIST(APPEND SINGLECPU_SOURCES "${SOURCES_DIR}/SingleCPUKernel.cpp")
LIST(APPEND SINGLECPU_SOURCES "${SOURCES_DIR}/SingleCPUProgramFactory.cpp")
LIST(APPEND SINGLECPU_SOURCES "${SOURCES_DIR}/SingleCPUKernelStateModel.cpp")

# --- programs ---
LIST(APPEND SINGLECPU_SOURCES "${SOURCES_DIR}/programs/SingleCPUTestProgram.cpp")
LIST(APPEND SINGLECPU_SOURCES "${SOURCES_DIR}/programs/SingleCPUAddParticleProgram.cpp")
LIST(APPEND SINGLECPU_SOURCES "${SOURCES_DIR}/programs/SingleCPUDiffuseProgram.cpp")

# --- potentials ---
LIST(APPEND SINGLECPU_SOURCES "${SOURCES_DIR}/potentials/PotentialsOrder1.cpp")

# --- model ---
LIST(APPEND SINGLECPU_SOURCES "${SOURCES_DIR}/model/SingleCPUParticleData.cpp")
