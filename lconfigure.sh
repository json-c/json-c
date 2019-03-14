#! /usr/bin/env bash

## *****************************************************************************
##  
## Description : lconfigure is a script used for compiling third-party autotools
##               projects with custom flags for Linux.
##
## *****************************************************************************
##/*===============================================================================*/
##/* Copyright (c) 2019 Elear Solutions Tech Private Limited. All rights reserved. */
##/* To any person (the "Recipient") obtaining a copy of this software and         */
##/* associated documentation files (the "Software"):                              */
##/*                                                                               */
##/* All information contained in or disclosed by this software is confidential    */
##/* and proprietary information of Elear Solutions Tech Private Limited and all   */
##/* rights therein are expressly reserved. By accepting this material the         */
##/* recipient agrees that this material and the information contained therein is  */
##/* held in confidence and in trust and will NOT be used, copied, modified,       */
##/* merged, published, distributed, sublicensed, reproduced in whole or in part,  */
##/* nor its contents revealed in any manner to others without the express         */
##/* written permission of Elear Solutions Tech Private Limited.                   */
##/*===============================================================================*/

make distclean
autoreconf -fsi
./configure CFLAGS="-fPIC"
make
sudo make install
sudo ldconfig
