## 后端配置需求

- **文件位置要求**

  - 运行`main.py`文件，要求`devices.json`文件与`main.py`在同一文件夹下

  - `main.py`调用`src/ptest`可执行文件生成布局布线结果文件`output.txt`，可根据需求自行调整

   ```python
    # 执行可执行文件并获取结果文件
    result_file = './output.txt'
    net_tm = '-all'
    process = subprocess.run(['../src/ptest', file_path, net_tm], capture_output=True)
   ```
  
- **关闭防火墙**

- **端口：**前端向后端**8080**端口发送请求，如果使用`WSL2`，则需**配置转发**：

  管理员身份允许`Powershell` 

  ```powershell
  # xxx.xxx.xxx.xxx为WSL2的IP地址，通过ifconfig指令获取
  netsh interface portproxy add v4tov4 listenport=8080 connectaddress=xxx.xxx.xxx.xxx connectport=8080
  # 配置防火墙添加加允许入站规则
  New-NetFirewallRule -DisplayName "Allow Inbound TCP Port 8080" -Direction Inbound -Action Allow -Protocol TCP -LocalPort 8080
  # 查看端口转发是否配置成功
  netsh interface portproxy show v4tov4
  # 删除配置
  netsh interface portproxy delete v4tov4 listenport=8080
  Remove-NetFirewallRule -DisplayName "Allow Inbound TCP Port 8080"
  ```

  端口转发配置成功显示如下：

  ![port-resend](C:\Users\dingyanfeng\Desktop\软件使用\port-resend.png)

  ***需要注意：WSL2的地址每次开启可能会变化，可能需要重新配置转发*** 

## 前端使用需求

- 如何运行：双击可执行文件直接运行，双击后可能会在当前目录下配置一些资源文件，建议新建一个文件夹后再放入可执行文件双击
- **Server IP**如何填写：填写后端的IP地址，如果后端运行在`WSL2`上，则通过`ipconfig`获取IP地址（不要填写`WSL2`的IP，已经配置转发）

## 前端用户手册

- 首先点击按钮跳出弹窗，在弹窗中填写服务器IP地址，若为本机则填写`127.0.0.1`

  <img src="C:\Users\dingyanfeng\Desktop\软件使用\step1.png" alt="step1" style="zoom:67%;" />

- 然后点击`Upload file`按钮上传以`.spi`为后缀的文件

- 后端返回成功后会在下拉菜单栏显示上传的`.spi`文件中所有的网表，可以通过点击选择显示的网表（默认为最后一张网表）

  <img src="C:\Users\dingyanfeng\Desktop\软件使用\bar.png" alt="bar" style="zoom:80%;" />

- 一个可能的结果页面显示如下：用户可以通过**鼠标点击页面实现拖拽**、**`Ctrl+鼠标滚轮`实现页面缩放**、**鼠标右键点击元件实现跳转**（若无该元件对应的子网表则会显示'未找到对应网表'的弹窗）。

  <img src="C:\Users\dingyanfeng\Desktop\软件使用\example.png" alt="example" style="zoom:80%;" />
