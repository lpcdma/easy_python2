adb push hook.py /data/local/tmp/hook.py
adb shell su -c "cp /data/local/tmp/hook.py /data/data/com.netease.yhtj/files/hook.py" 
adb shell su -c "chmod 777 /data/data/com.netease.yhtj/files/hook.py" 