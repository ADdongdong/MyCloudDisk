# MyCloudDisk
 Qt网盘前端项目
##  登录界面

### 1.1 界面1：用户登录界面
![image](https://github.com/user-attachments/assets/1d397bea-3d1e-47c8-a27d-15bb81dae76c)

### 1.2 界面2：设置界面，可设置服务器的ip和port
![image](https://github.com/user-attachments/assets/d4e1db92-ebb8-4f26-9879-9478b3c56095)

### 1.3 界面3：注册界面
![image](https://github.com/user-attachments/assets/c1064772-9607-4273-bdcd-9c8426bdc9e9)

## 主界面

### 2.1 文件界面
> 文件界面可以展示当前用户存储在分布式服务器上的文件内容，可以完成文件上传操作、文件删除操作、界面刷新操作。
![image](https://github.com/user-attachments/assets/6e2b0d9c-71c5-4396-8fc8-e480bd48fc9c)

### 2.2 共享列表
>不同的用户可以将自己想分享的文件放入到共享列表中，工其他用户下载，共享列表中的文件，在服务器中只保存一份，该功能又引用计数实现，每当有用户删除该文件，该文件的引用计数-1，当所有用户都删除该文件后，该文件从服务器彻底删除。
![image](https://github.com/user-attachments/assets/127401da-3677-4991-bab3-62bed753b50e)

### 2.3下载榜
>该文件获取共享列表中所有用户所上传的共享文件，其他所有用户下载的次数。并按下载次数从大大小排序。
![image](https://github.com/user-attachments/assets/45eaae88-2bd4-48a6-9b80-b653d264cd7e)

### 2.4 传输列表
>该页面可展示3个信息，文件下载的进度条，文件上传进度条和传输记录。
![image](https://github.com/user-attachments/assets/efd122a8-515e-42ec-9b94-8d50c34742d2)

### 2.5 切换用户
点击切换用户按钮，会返回到登录界面，重新登录，以实现切换用户的功能。
