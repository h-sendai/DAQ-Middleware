#!/bin/sh

set -e

sed -e \
'/@one_comp@/ {
    s/@one_comp@//g
    r images/one.gif.base64
}' \
-e \
'/@two_comp@/ {
    s/@two_comp@//g
    r images/two.gif.base64
}' \
-e \
'/@four_comp@/ {
    s/@four_comp@//g
    r images/four.gif.base64
}' \
confPanel.py.in > confPanel.py
