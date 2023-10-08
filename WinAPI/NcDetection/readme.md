# NC异常检测(NCQuery)及修复(NcRecovery)说明

[toc]

## 1. NC异常检测(NCQuery)

### 检测模块

1. 虚拟网卡(netcard)
   
   > 检查虚拟网卡的状态（禁用、启用、未启用）
2. 通信服务及部分操作(ncopt)
   
   > 与 `Trustcore` 的通信及网卡启动前操作
3. vnic
   
   > 检查虚拟ip的分配是否成功
4. 路由
   
   > 检查路由表>



### 返回结构

~~~json
{
    "error_code": 0,
    "message": "Configure vnic Success",
    "modules": [
        {
            "level": "INFO",
            "message": "Create remote vnip success",
            "name": "ncip",
            "status": "0",
            "time": " 2023-04-07T10:33:14Z"
        },
        {
            "level": "INFO",
            "message": "Get & Set OPtions Success",
            "name": "ncopt",
            "status": "0",
            "time": " 2023-04-07T10:33:14Z"
        },
        {
            "level": "INFO",
            "message": "Configure vnic Success",
            "name": "netcard",
            "options": "{\"data\": {\"block_exclude_lan\": 0, \"block_internet\": 0, \"dns\": [], \"dns4\": [], \"dns6\": [], \"dns_suffix\": \"\", \"ip\": \"1.1.1.114\", \"ipv4\": \"1.1.1.114\", \"mask\": \"255.255.255.255\", \"route_assign4\": [], \"route_assign6\": [], \"route_auto\": \"1\", \"route_includes4\": [\"1.1.1.1/32\", \"10.41.3.204/32\", \"10.41.3.222/32\", \"10.44.250.1/32\", \"10.44.250.2/31\", \"10.44.250.4/30\", \"10.44.250.8/29\", \"10.44.250.16/28\", \"10.44.250.32/27\", \"10.44.250.64/26\", \"10.44.250.128/30\", \"10.44.250.134/31\", \"10.44.250.136/29\", \"10.44.250.144/28\", \"10.44.250.160/27\", \"10.44.250.192/27\", \"10.44.250.224/28\", \"10.44.250.240/29\", \"10.44.250.248/30\", \"10.44.250.252/31\", \"10.44.250.254/32\", \"10.92.2.33/32\", \"10.92.2.250/32\", \"10.92.3.132/32\", \"198.18.0.0/30\", \"198.18.0.4/31\", \"198.18.0.6/32\"], \"route_opt\": \"0\", \"vpn_host\": \"10.44.250.133\", \"vpn_ip\": \"10.44.250.133\", \"vpn_ip4\": \"10.44.250.133\", \"vpn_port\": \"443\"}, \"errcode\": \"0\"}",
            "status": "0",
            "time": " 2023-04-07T10:33:22Z"
        },
        {
            "level": "INFO",
            "message": "Add route table Success",
            "name": "route",
            "routetable": "[\"1.1.1.1/32\", \"10.41.3.204/32\", \"10.41.3.222/32\", \"10.44.250.1/32\", \"10.44.250.2/31\", \"10.44.250.4/30\", \"10.44.250.8/29\", \"10.44.250.16/28\", \"10.44.250.32/27\", \"10.44.250.64/26\", \"10.44.250.128/30\", \"10.44.250.134/31\", \"10.44.250.136/29\", \"10.44.250.144/28\", \"10.44.250.160/27\", \"10.44.250.192/27\", \"10.44.250.224/28\", \"10.44.250.240/29\", \"10.44.250.248/30\", \"10.44.250.252/31\", \"10.44.250.254/32\", \"10.92.2.33/32\", \"10.92.2.250/32\", \"10.92.3.132/32\", \"198.18.0.0/30\", \"198.18.0.4/31\", \"198.18.0.6/32\"]",
            "status": "0",
            "time": " 2023-04-07T10:33:22Z"
        }
    ],
    "os": "windows",
    "service": "trustnc",
    "state": "R",
    "time": " 2023-04-07T10:33:22Z"
}
~~~

#### netcard

| 参数    | 父节点  | 类型   | 说明                         |
| ------- | ------- | ------ | ---------------------------- |
| netcard | \       |        | 返回虚拟网卡相关状态         |
| level   | netcard | string | 返回异常状态的级别           |
| status  | netcard | string | 返回虚拟网卡状态的状态码     |
| options | Netcard | string | (独有)返回虚拟网卡的配置信息 |
| message | netcard | string | 返回状态码对应的描述信息     |

#### ncip

| 参数    | 父节点 | 类型   | 说明                     |
| ------- | ------ | ------ | ------------------------ |
| ncip    | \      |        | 返回虚拟ip相关状态       |
| level   | ncip   | string | 返回异常状态的级别level  |
| status  | ncip   | string | 返回虚拟ip的状态码       |
| message | ncip   | string | 返回状态码对应的描述信息 |

#### ncopt

| 参数    | 父节点 | 类型   | 说明                     |
| ------- | ------ | ------ | ------------------------ |
| ncopt   | \      |        | 返回nc通信的状态         |
| level   | ncopt  | string | 返回异常状态的级别       |
| status  | ncopt  | string | 返回通信状态的状态码     |
| message | ncopt  | string | 返回状态码对应的描述信息 |
#### route

| 参数       | 父节点 | 类型   | 说明                         |
| ---------- | ------ | ------ | ---------------------------- |
| route      | \      |        | 返回路由信息                 |
| status     | route  | string | 返回路由的状态码             |
| level      | route  | string | 返回异常状态的级别           |
| message    | route  | string | 返回路由状态码对应的描述信息 |
| routetable | route  | string | 返回路由表的json字符串       |


### Status 说明

1. 虚拟网卡
2. 通信服务
3. vnic
4. 路由

 

#### 1. 虚拟网卡[0,1,2~100]

| level | status | message                                                      | 描述                                      | 处置动作                 |
| ----- | ------ | ------------------------------------------------------------ | ----------------------------------------- | ------------------------ |
|       | 0      |                                                              | 正常状态                                  |                          |
|       | 0      | 1. opened                                                    | 正常状态：启用虚拟网卡成功                |                          |
|       | 0      | 2. HKLM Query Class Success                                  | 正常状态：                                |                          |
|       | 0      | 3. HKLM Query Network Success                                | 正常状态：                                |                          |
|       | 0      | 4. Find Friendly Name                                        | 正常状态：                                |                          |
|       |        |                                                              |                                           |                          |
|       | 0      | Connect has started                                          | 正常状态：                                |                          |
|       | 1      | unknown                                                      | 初始化状态                                |                          |
|       | 2      | Not Found Device Vnic                                        | 未找到vnic设备                            | 重新安装                 |
|       | 3      | Configure vnic failed                                        | 设置网卡信息失败（ip、网关、掩码、dns等） | 重新获取网卡配置并设置   |
|       | 4      | [HKLM Query Faild](https://wiki.qianxin-inc.cn/pages/viewpage.action?pageId=712315064) | 查询注册表失败                            | 一般情况下还是网卡未安装 |
|       |        |                                                              |                                           |                          |
|       |        |                                                              |                                           |                          |
|       |        |                                                              |                                           |                          |



#### 2. 通信及操作(ncopt)[0,1,101~200]

| level | status | message                                         | 描述                   | 处置动作     |
| ----- | ------ | ----------------------------------------------- | ---------------------- | ------------ |
| INFO  | 0      | 1. normal<br />                                 | 正常状态               |              |
| INFO  | 0      | Get & Set OPtions Success                       | 获取配置和设置句柄成功 |              |
|       | 0      | Configure vnic Success                          | 正常状态：连接core成功 |              |
| INFO  | 0      | Connect has started                             | 虚拟网卡已经启动       | ？           |
| INFO  | 1      | unknown                                         | 初始化状态             |              |
| ERROR | 101    | Send initial msg to vpncore failed              | 发送PTUN请求失败       | 重新获取配置 |
| ERROR | 102    | Send initial msg to vpncore failed with option  | 发送默认路由失败       | 重新获取配置 |
| ERROR | 103    | Recv msg header from vpncore failed             | 接收PTUN消息失败       | 重新获取配置 |
| ERROR | 104    | Alloc for msg_body failed                       | 初始化内存失败         | 重新获取配置 |
| ERROR | 105    | Recv initial msg response from vpnclient failed | 接收消息失败           | 重新获取配置 |
| ERROR | 106    | NC options is null                              | 获取NC配置为空         | ?            |
| ERROR | 107    | Set Handler Failed                              | 设置控制句柄失败       | 重新获取配置 |
|       |        |                                                 |                        |              |
|       |        |                                                 |                        |              |
|       |        |                                                 |                        |              |



#### 3. vnic[0,1,201~300]

| level | status | message                 | 描述           | 处置动作                         |
| ----- | ------ | ----------------------- | -------------- | -------------------------------- |
| INFO  | 0      | 1.                      | 正常状态       |                                  |
| INFO  | 1      | unknown                 | 初始化状态     |                                  |
| ERROR | 201    | Create remote ip failed | 创建虚拟ip失败 | 重新创建且重新走开启虚拟网卡流程 |



#### 4. 路由表[0,1,301~400]

| level | status | message                   | 描述                       | 处置动作 |
| ----- | ------ | ------------------------- | -------------------------- | -------- |
| INFO  | 0      |                           | 正常状态                   |          |
| INFO  | 0      | Add route table Success   | 添加路由（同时保存路由表） |          |
| INFO  | 1      | unknown                   | 初始化状态                 |          |
| ERROR | 301    | Add route table failed    | 添加路由失败               | 重新添加 |
|       | 302    | Repair route table failed | 修复路由失败               |          |
|       |        |                           |                            |          |
|       |        |                           |                            |          |
|       |        |                           |                            |          |
|       |        |                           |                            |          |



## 2. NC异常修复(NcRecovery)



异常修复针对针对上一节中的 Status 对应的处置动作详细说明。对nc内部发生的异常可供内部修复的项目进行处理。



~~~json
{
    "ncip": {
        "message": "unknown",
        "status": "1"
    },
    "ncopt": {
        "message": "unknown",
        "status": "1"
    },
    "netcard": {
        "level": "INFO",
        "message": "Add route table Success",
        "options": "{\"L4\": {\"hostname_excludes\": [], \"hostname_includes\": [\"www.test.com\", \"www.qianxin.com\", \"aa.com\"], \"proxy_port\": 50568, \"proxy_rules\": [{\"action\": \"bypass\", \"ip_max\": \"10.44.250.132\", \"ip_min\": \"10.44.250.132\", \"port_max\": 443, \"port_min\": 443, \"protocol\": \"tcp\"}, {\"action\": \"bypass\", \"ip_max\": \"10.44.250.132\", \"ip_min\": \"10.44.250.132\", \"port_max\": 443, \"port_min\": 443, \"protocol\": \"tcp\"}, {\"action\": \"bypass\", \"ip_max\": \"10.44.250.133\", \"ip_min\": \"10.44.250.133\", \"port_max\": 443, \"port_min\": 443, \"protocol\": \"tcp\"}, {\"action\": \"redirect\", \"family\": \"ipv4\", \"ip_max\": \"1.1.1.1\", \"ip_min\": \"1.1.1.1\", \"port_max\": 3, \"port_min\": 3, \"protocol\": \"tcp\"}, {\"action\": \"redirect\", \"family\": \"ipv4\", \"ip_max\": \"10.44.250.134\", \"ip_min\": \"10.44.250.134\", \"port_max\": 80, \"port_min\": 80, \"protocol\": \"tcp\"}, {\"action\": \"redirect\", \"family\": \"ipv4\", \"ip_max\": \"10.92.2.33\", \"ip_min\": \"10.92.2.33\", \"port_max\": 80, \"port_min\": 80, \"protocol\": \"tcp\"}, {\"action\": \"redirect\", \"family\": \"ipv4\", \"ip_max\": \"198.18.255.255\", \"ip_min\": \"198.18.0.0\", \"port_max\": 65535, \"port_min\": 0, \"protocol\": \"tcp\"}, {\"action\": \"redirect\", \"family\": \"ipv4\", \"ip_max\": \"255.255.255.255\", \"ip_min\": \"0.0.0.0\", \"port_max\": 53, \"port_min\": 53, \"protocol\": \"udp\"}, {\"action\": \"redirect\", \"family\": \"ipv6\", \"ip_max\": \"FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF:FFFF\", \"ip_min\": \"::\", \"port_max\": 53, \"port_min\": 53, \"protocol\": \"udp\"}]}, \"data\": {\"block_exclude_lan\": 0, \"block_internet\": 0, \"dns\": [], \"dns4\": [], \"dns6\": [], \"dns_suffix\": \"\", \"ip\": \"1.1.1.29\", \"ipv4\": \"1.1.1.29\", \"mask\": \"255.255.255.255\", \"route_assign4\": [], \"route_assign6\": [], \"route_auto\": \"1\", \"route_includes4\": [\"1.1.1.1/32\", \"10.44.250.130/32\", \"10.44.250.134/32\", \"10.92.2.33/32\", \"198.18.0.0/31\", \"198.18.0.2/32\"], \"route_opt\": \"0\", \"vpn_host\": \"10.44.250.133\", \"vpn_ip\": \"10.44.250.133\", \"vpn_ip4\": \"10.44.250.133\", \"vpn_port\": \"443\"}, \"errcode\": \"0\"}",
        "routetable": "[\"1.1.1.1/32\", \"10.44.250.130/32\", \"10.44.250.134/32\", \"10.92.2.33/32\", \"198.18.0.0/31\", \"198.18.0.2/32\"]",
        "status": "0"
    },
    "recv": {
        "route": {
            "status": "1"
        }
    },
    "route": {
        "message": "unknown",
        "status": "1"
    }
}
~~~





## 触发nc的操作

~~~cpp
    SP_TAP_STAGE_CREATE         = 0x00,     // Created
    SP_TAP_STAGE_ACCEPT_APP     = 0x01,     // Accept APP end's connection, or connected backend APP server
    // TODO: more stage for PROXY supports: proxy-connecting proxy-handshaking
    SP_TAP_STAGE_CONNECT_LOOKUP = 0x02,     // DNS lookup, resolving remote host
    SP_TAP_STAGE_CONNECT_TCP    = 0x03,     // connecting remote host by TCP
    SP_TAP_STAGE_CONNECT_SSL    = 0x04,     // connecting remote host by SSL or SSL handshaking on exists TCP
    SP_TAP_STAGE_HANDSHAKE      = 0x05,     // has connected SSLVPN server authorizing
    SP_TAP_STAGE_FORWARD        = 0x06,     // has authorized SSLVPN Server forwarding data
    SP_TAP_STAGE_IDLE           = 0x07,     // has NO L3 services, nc tunnel is not connected, idle state
    // SP_TAP_STAGE_ERROR          = 0x7E,
    SP_TAP_STAGE_CLOSED         = 0x7F
~~~



触发 SP_TAP_STAGE_FORWARD 的操作

- OnTapAppRead `stage!=SP_TAP_STAGE_FORWARD` 已认证需要tap数据

- SPTapTunnelBypass::OnTapVpnEvent `stage<SP_TAP_STAGE_HANDSHAKE` 已连接但未认证
- SPTapTunnelRelay::OnTapVpnEvent `stage== BEV_EVENT_CONNECTED && stage<SP_TAP_STAGE_HANDSHAKE` 已连接但未认证

- SPTapTrustTunnelTCP::OnHandshakeRsp接口 `errorcode == 0`

- SPTapTrustTunnelIP::OnHandshakeRsp接口`errorcode == 0` 
- SPTapAgent::OnTapVpnRead `隧道建立成功 && stage==SP_TAP_STAGE_HANDSHAKE`
- SPTapAgent::OnTapVpnRead `tag == SP_MSG_VPN_AGT_SESSION_START` 会话开始
- SPTapAgentHub::OnRspMsgBinary `tag==SP_MSG_VPN_AGT_PROXY_AUTH` 服务访问授权
- SPTapUDPGateway::OnTapAppRead `stage<SP_TAP_STAGE_FORWARD` 
- SPTapTrustRESTful::OnTapVpnRead `stage == SP_TAP_STAGE_HANDSHAKE`
- SPTapTunnelHTTPD::OnTapAppRead `stage!=SP_TAP_STAGE_FORWARD` 
- SPTapTunnelNC::OnTapVpnRead `stage==SP_TAP_STAGE_FORWARD`
- SPTapTunnelMsg::OnTapVpnEvent `stage==SP_TAP_STAGE_CONNECT_SSL`

 







# 测试建议

## 设置网卡

> 对应下发节点（data节点）：
>
> - ip 虚拟路由
> - ipv4 虚拟路由
> - mask 掩码
> - vpn_ip 网关

### Win7及以后

#### #1
~~~shell
netsh interface ip set address 14 static 1.1.1.95 255.255.255.255 store=active
# netsh interface ip set address ? # 查看详细帮助
~~~
- 14: 为 “TrusgAgent VNIC” 的索引（此处也可以设置为名称）。
- 1.1.1.95: 为下发的虚拟ip
- 255.255.255.255: 为掩码
- store: `active` 时表示设置仅持续到下一次启动；`persistent` ，设置永久有效

#### #2
~~~shell
netsh interface ip set interface interface=14 metric=1 store=active
# netsh interface ip set interface ? #查看帮助
~~~

- interface: 接口名称或索引
- metric: 接口跃点数，已添加到接口上的所有路由的路由跃点数
- store: 同【#1】


#### #3
~~~shell
netsh interface ip delete dns 14 all
~~~
netsh interface  ip delete ?

下列指令有效:

此上下文中的命令:
delete address - 从指定的接口删除 IP 地址或默认网关。
delete arpcache - 刷新一个特定接口或所有接口的 ARP 缓存。
delete destinationcache - 删除目标缓存。
delete dnsservers - 从指定的接口删除 DNS 服务器。
delete excludedportrange - 为连续的端口块删除排除。
delete neighbors - 刷新一个特定接口或所有接口的 ARP 缓存。
delete route   - 删除路由。
delete winsservers - 从指定的接口删除 WINS 服务器。

#### #4

添加dns服务武器IP和wins服务器IP

~~~shell
# 添加 dns 服务器IP
netsh interface ip add dns 14 %s validate = no
# 添加 Wins 服务器IP
netsh interface ip add wins %d %s
~~~



### 其他

#### #1

~~~shell
netsh interface ip set address 以太网 dhcp
~~~

- 以太网： 网络名称
- dhcp：动态路由

#### #2

通过系统API添加(虚拟ip、目标ip、掩码等）

~~~cpp
DeviceIoControl
~~~

#### #3

通过系统API设置dns



## 添加路由

> 对应下发节点： data > route_includes

vnic执行的命令如下所示：

~~~shell
ADD %ip% MASK %mask% %remoteip%
~~~

- ipv4
  - ip: 获取配置字段中的`vpn_ip`，一般为【tap地址】
  - mask:
  - remotip
- ipv6
  - ip: 同【ipv4】
  - mask: 同 【ipv4】
  - remoteip:添加ipv6路由时，远端ip为 `"20.21.8.20"`



## 删除路由

使用 `Route Print` 查看活动路由

~~~shell
route DELETE %s MASK %s
# 示例：删除[网络目标]为 10.44.250.144,[网络掩码]为 255.255.255.240的条目
# route delete 10.44.250.144 mask 255.255.255.240
~~~















