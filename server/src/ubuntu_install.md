# ubuntu 编译


# 安装mysql 替代mariadb(已安装可以忽略)
```
sudo apt-get update
sudo apt-get install mysql-server libmysqlclient-dev

#配置用户密码
sudo mysql_secure_installation

```

# 编译依赖
```
./third_party/make_hireds.sh
./third_party/make_log4cxx.sh
./third_party/make_protobuf.sh
```

# 修改点编译配置

编辑server/src/db_proxy_server/CMakeLists.txt
最后一行替换 mysqlclient_r 为 mysqlclient


```
./build.sh version test
```






