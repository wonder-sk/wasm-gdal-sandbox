#!/usr/bin/env pytest
# -*- coding: utf-8 -*-
###############################################################################
# $Id$
#
# Project:  GDAL/OGR Test Suite
# Purpose:  Test read/write functionality for RST/Idrisi driver.
# Author:   Frank Warmerdam <warmerdam@pobox.com>
#
###############################################################################
# Copyright (c) 2006, Frank Warmerdam <warmerdam@pobox.com>
#
# Permission is hereby granted, free of charge, to any person obtaining a
# copy of this software and associated documentation files (the "Software"),
# to deal in the Software without restriction, including without limitation
# the rights to use, copy, modify, merge, publish, distribute, sublicense,
# and/or sell copies of the Software, and to permit persons to whom the
# Software is furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS
# OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL
# THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
# FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
# DEALINGS IN THE SOFTWARE.
###############################################################################

import os

import gdaltest

###############################################################################
# Read test of byte file.


def test_idrisi_1():

    tst = gdaltest.GDALTest("RST", "rst/byte.rst", 1, 5044)
    tst.testOpen()


###############################################################################
# Read test of byte file.


def test_idrisi_2():

    tst = gdaltest.GDALTest("RST", "rst/real.rst", 1, 5275)
    tst.testOpen()


###############################################################################
#


def test_idrisi_3():

    tst = gdaltest.GDALTest("RST", "ehdr/float32.bil", 1, 27)

    tst.testCreate(new_filename="tmp/float32.rst", out_bands=1, vsimem=1)


###############################################################################
#


def test_idrisi_4():

    tst = gdaltest.GDALTest("RST", "rgbsmall.tif", 2, 21053)

    tst.testCreateCopy(
        check_gt=1, check_srs=1, new_filename="tmp/rgbsmall_cc.rst", vsimem=1
    )


###############################################################################
# Cleanup.


def test_idrisi_cleanup():
    gdaltest.clean_tmp()
    try:
        os.unlink("data/rgbsmall.tif.aux.xml")
        os.unlink("data/rst/real.rst.aux.xml")
        os.unlink("data/frmt09.cot.aux.xml")
        os.unlink("data/rst/byte.rst.aux.xml")
        print("FIXME?: data/rgbsmall.tif.aux.xml is produced by those tests")
    except OSError:
        pass
