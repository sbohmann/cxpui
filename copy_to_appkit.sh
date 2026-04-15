#!/bin/sh
set -e
cp main.c cxpui.* core.* view_list.* uikit/cxpui-uikit/cxpui-uikit/
cp errors/*.c errors/*.h uikit/cxpui-uikit/cxpui-uikit/errors/
date
echo "copied"
