#!/bin/bash

gcc -o raft_2 raft_2.c -pthread -lnsl

# 获取rpcinfo输出并使用awk筛选需要删除的服务
services_to_delete=$(rpcinfo -p | awk 'NR > 1 && $1 != 100000 && $3 == "udp" { print $1 " " $2 }')

# 使用read命令将每一行的两个值分配给prognum和versnum，并执行其他操作
echo "$services_to_delete" | while read -r prognum versnum; do
  # 可以在此处执行其他操作，例如删除服务
  sudo rpcinfo -d $prognum $versnum
done


