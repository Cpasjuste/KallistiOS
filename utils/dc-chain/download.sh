#!/usr/bin/env bash

# Getting configuration from Makefile
source ./scripts/common.sh

print_banner "Downloader"

while [ "$1" != "" ]; do
    PARAM=`echo $1 | awk -F= '{print $1}'`
    case $PARAM in
        --config-guess-only)
            CONFIG_GUESS_ONLY=1
            ;;
        *)
            echo "error: unknown parameter \"$PARAM\""
            exit 1
            ;;
    esac
    shift
done

function download()
{
  local name=$1
  local ver=$2
  local url=$3
  local filename=$(basename $url)

  if [ -n "$ver" ]; then
    if [ ! -f $filename ]; then
      echo "Downloading ${name} ${ver}..."
      ${WEB_DOWNLOADER} "${DOWNLOAD_PROTOCOL}${url}" || exit 1
    else
      echo "$name $ver was already downloaded"
	fi
  fi
}

function download_dependencies()
{
  local arch=$1

  local gmp_ver=$SH_GMP_VER
  local mpfr_ver=$SH_MPFR_VER
  local mpc_ver=$SH_MPC_VER
  local isl_ver=$SH_ISL_VER
  local gmp_url=$SH_GMP_URL
  local mpfr_url=$SH_MPFR_URL
  local mpc_url=$SH_MPC_URL
  local isl_url=$SH_ISL_URL

  if [ "$arch" == "arm" ]; then
    gmp_ver=$ARM_GMP_VER
    mpfr_ver=$ARM_MPFR_VER
    mpc_ver=$ARM_MPC_VER
    isl_ver=$ARM_ISL_VER
    gmp_url=$ARM_GMP_URL
    mpfr_url=$ARM_MPFR_URL
    mpc_url=$ARM_MPC_URL
    isl_url=$ARM_ISL_URL
  fi

  if [ "$USE_CUSTOM_DEPENDENCIES" == "1" ]; then
    download "GMP"  "$gmp_ver"   "$gmp_url"
    download "MPFR" "$mpfr_ver"  "$mpfr_url"
    download "MPC"  "$mpc_ver"   "$mpc_url"
    download "ISL"  "$isl_ver"   "$isl_url"
  fi
}

# Download everything.
if [ -z "${CONFIG_GUESS_ONLY}" ]; then
  # Downloading SH components
  download "Binutils" "$SH_BINUTILS_VER" "$SH_BINUTILS_URL"
  download "GCC" "$SH_GCC_VER" "$SH_GCC_URL"
  download_dependencies "sh"
  download "Newlib" "$NEWLIB_VER" "$NEWLIB_URL"

  # Downloading ARM components
  download "Binutils" "$ARM_BINUTILS_VER" "$ARM_BINUTILS_URL"
  download "GCC" "$ARM_GCC_VER" "$ARM_GCC_URL"
  download_dependencies "arm"
fi

# Downloading config.guess.
if [ ! -f ${CONFIG_GUESS} ]; then
  WEB_DOWNLOAD_OUTPUT_SWITCH="-O"
  if [ ! -z "${IS_CURL}" ] && [ "${IS_CURL}" != "0" ]; then
    WEB_DOWNLOADER="$(echo ${WEB_DOWNLOADER} | cut -c-9)"
    WEB_DOWNLOAD_OUTPUT_SWITCH="-o"
  fi

  echo "Downloading ${CONFIG_GUESS}..."
  ${WEB_DOWNLOADER} ${WEB_DOWNLOAD_OUTPUT_SWITCH} ${CONFIG_GUESS} "http://git.savannah.gnu.org/gitweb/?p=config.git;a=blob_plain;f=${CONFIG_GUESS};hb=HEAD" || exit 1

  # This is needed for all systems except MinGW.
  chmod +x "./${CONFIG_GUESS}"
fi

echo "Done!"
