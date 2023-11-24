# Wireshark 报文抓取

## DNS 报文

查看本机 WLAN 的 DNS 服务器

```sh
ipconfig /all
```

找到无线局域网适配器下的 DNS 服务器栏，记录下 IP（可能存在多个，先拿第一个尝试，不行再换）。

> 建议顺便也看下本机 IP

打开 wireshark 选择无线网卡设备 WLAN，开始抓包，同时使用浏览器访问 www.baidu.com。加载完成后立即停止 wireshark 的抓取。

使用过滤器 `ip.src == 202.117.80.6 || ip.dst == 202.117.80.6`，找到目的地址为 DNS 服务器的报文并找到报文信息含 www.baidu.com 的包，这就是要求的 DNS 请求报文。

> 此处 202.117.80.6 替换成你的 DNS 服务器地址

找到对应的源地址为 DNS 服务器的报文，即为 DNS 响应报文。

大概长这样：

|No.|Time|Source|Destination|Protocol|Length|Info|
|:-:|:-:|:-:|:-:|:-:|:-:|:-|
|39|2.863990|10.31.5.62|202.117.80.6|DNS|73|Standard query 0xad34 A www.baidu.com|
|40|2.867692|202.117.80.6|10.31.5.62|DNS|135|Standard response 0xad34 A www.baidu.com CNAME www.a.shifen.com A 220.181.38.149 A 220.181.38.150|

其中响应报文中的 `220.181.38.149` 为百度服务器的 IP，可以通过 `nslookup www.baidu.com` 进行简单验证：

```plain
服务器:  UnKnown
Address:  202.117.80.6

非权威应答:
名称:    www.a.shifen.com
Addresses:  240e:ff:e020:966:0:ff:b042:f296
          240e:ff:e020:9ae:0:ff:b014:8e8b
          220.181.38.150
          220.181.38.149
Aliases:  www.baidu.com
```

> DNS 响应的地址可能会变，建议实时看下有没有更新

## ARP

记得清缓存（需要管理员执行）

```sh
arp -d *
```

> sh 可能会把 `*` 当成 glob，建议用 cmd 运行

学校校园网是华为机子的，看到 arp 报文的通信方含 `Huawei` 的基本就是了，另一个则是本机网卡设备。

## TCP 三次握手与四次挥手

TCP 三次握手流程：

1. 客户端发送 \[SYN] SEQ=c
2. 服务端响应 \[SYN, ACK] SEQ=s ACK=c+1
3. 客户端回复 \[ACK] SEQ=c+1 ACK=s+1

加强下过滤器，把本机 ip 加上 `(ip.src == 10.31.5.62 && ip.dst == 220.181.38.149) || (ip.src == 220.181.38.149 && ip.dst == 10.31.5.62)`。

> 10.31.5.62 替换成自己的本机 IP

重点观察信息含 SYN/ACK 的报文，最后得到的结果大概长这样（过滤后基本是挨一块的）：

|No.|Time|Source|Destination|Protocol|Length|Info|
|:-:|:-:|:-:|:-:|:-:|:-:|:-|
|42|2.868711|10.31.5.62|202.117.80.6|TCP|66|58015 -> 443 \[SYN] Seq=0 Win=64240 Len=0 MSS=1460 WS=256 SACK_PERM|
|43|2.892018|202.117.80.6|10.31.5.62|TCP|66|443 -> 58015 \[SYN, ACK] Win=8192 Len=0 MSS=1380 WS=32 SACK_PERM|
|44|2.892237|10.31.5.62|202.117.80.6|TCP|54|58015 -> 443 \[ACK] Seq=1 ACK=1 Win=131072 Len=0|

TCP 四次挥手流程

1. 客户端发送 \[FIN, ACK] SEQ=s1 ACK=c1
2. 服务器响应 \[ACK] SEQ=c1 ACK=s1+1
3. 服务器发送 \[FIN, ACK] SEQ=s2 ACK=c2
4. 客户端发送 \[ACK] SEQ=c2 ACK=s2+1

> 浏览器与网站建立连接没那么简单，关闭页面后连接可能仍未被关闭，可以将浏览器的所有进程全部关闭等待一段时间直到抓到 FIN。通过该方法掐断的 TCP 连接大概率抓取不到第四次挥手的报文，但是应该可以抓取到一个客户端发送给服务器的 \[RST, ACK]，该报文与服务器发送给客户端的 \[FIN, ACK] 报文成对出现，且互相的接受序号与确认序号相等。
