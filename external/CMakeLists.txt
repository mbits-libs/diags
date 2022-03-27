if (NOT TARGET semver AND NOT TARGET mbits::libsemver)
add_subdirectory(semver)
endif()
