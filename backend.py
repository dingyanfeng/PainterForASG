from flask import Flask, request, send_file
import subprocess
from gevent import pywsgi
import argparse
import os
import signal
from art import text2art
import uuid
# 支持异步非阻塞并发
from gevent import monkey
monkey.patch_all()

# from flask_cors import CORS

app = Flask(__name__)
# CORS(app, supports_credentials=True)

class ColorLogDecorator:
    __IS_ACTIVE: bool = False
    __DEFAULT_STYLE: str = "normal"
    __COLOR_CODE: dict = {
        "red": {
            "normal": "\033[31m",
            "strong": "\033[1;31m",
            "bg": "\033[0;37;41m",
            "bg-strong": "\033[1;37;41m",
        },
        "green": {
            "normal": "\033[32m",
            "strong": "\033[1;32m",
            "bg": "\033[0;37;42m",
            "bg-strong": "\033[1;37;42m",
        },
        "yellow": {
            "normal": "\033[33m",
            "strong": "\033[1;33m",
            "bg": "\033[0;30;43m",
            "bg-strong": "\033[1;30;43m",
        }
    }
    __END_CODE: str = "\033[0m"

    @classmethod
    def active(cls) -> None:
        cls.__IS_ACTIVE = True
        if os.name == "nt":
            os.system("")

    @classmethod
    def deactivate(cls) -> None:
        cls.__IS_ACTIVE = False

    @classmethod
    def __msg_decorator(cls, msg: str, color: str, style: str) -> str:
        if not cls.__IS_ACTIVE:
            return msg

        style_selected: str = cls.__DEFAULT_STYLE if style.lower() not in cls.__COLOR_CODE[color].keys() \
            else style.lower()

        return cls.__COLOR_CODE[color][style_selected] + msg + cls.__END_CODE

    @classmethod
    def red(cls, msg: str, style: str = "normal") -> str:
        return cls.__msg_decorator(msg, "red", style)

    @classmethod
    def green(cls, msg: str, style: str = "normal") -> str:
        return cls.__msg_decorator(msg, "green", style)

    @classmethod
    def yellow(cls, msg: str, style: str = "normal") -> str:
        return cls.__msg_decorator(msg, "yellow", style)


@app.route('/upload', methods=["POST"])
def upload_file():
    ColorLogDecorator.active()
    print(ColorLogDecorator.green('Establish a connection!'))

    request_id = str(uuid.uuid4())
    print(f"Received request with ID: {request_id}")

    # 检查Content-Type是否为multipart/form-data
    content_type = request.headers.get('Content-Type')
    if not content_type.startswith('multipart/form-data'):
        return 'Invalid Content-Type. Expected multipart/form-data.', 400

    # 打印请求中的所有字段名称
    print(f"Received fields: {list(request.files.keys())}")
    
    # 检查文件字段名称是否匹配
    if 'file' not in request.files.keys():
        return 'Missing file field in the request.', 400

    # 用requestid创建运行文件夹并准备运行所需文件
    mkdir = subprocess.run(['mkdir', request_id], capture_output=True, text=True)

    # 接收前端传输的文件
    file = request.files['file']

    # 保存上传的文件到服务器
    file_path = './{}/file.spi'.format(request_id)
    file.save(file_path)
    print('We have save the file.')

    # 执行可执行文件并获取结果文件
    # result_file = './output.txt'
    result_file = os.path.abspath('./{}/output.txt'.format(request_id))
    net_tm = '-all'
    process = subprocess.run([args.exe, file_path, net_tm, request_id], capture_output=True)

    if process.returncode != 0:
        error_message = f"Subprocess returned non-zero exit code: {process.returncode}"
        remove_all = subprocess.run(['rm', '-rf', './{}'.format(request_id)], capture_output=True, text=True)
        return error_message, 500

    print('We have done the exe.')
    ColorLogDecorator.deactivate()

    # 将结果文件传回前端
    response = send_file(result_file, as_attachment=True)
    
    # 删除文件
    remove_all = subprocess.run(['rm', '-rf', './{}'.format(request_id)], capture_output=True, text=True)
    # 将结果文件传回前端
    return response


def stop_server(signum, frame):
    global server
    ColorLogDecorator.active()
    if server is not None:
        server.stop()
        print(ColorLogDecorator.red(' Server stopped.'))
    ColorLogDecorator.deactivate()


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('--port', type=int, default=8080, help='The port number to run the server on.')
    parser.add_argument('--exe', type=str, default='./placement-and-routing/serial-asg', help='The executable file to run for placement and routing.')

    args = parser.parse_args()
    
    ColorLogDecorator.active()
    print(ColorLogDecorator.green(text2art("Schematic Generation")))
    print(f"The placement and routing Executable is: {ColorLogDecorator.green(args.exe)}")
    print(f"Running on Port                        : {ColorLogDecorator.green(str(args.port))}")

    server = pywsgi.WSGIServer(('0.0.0.0', args.port), app)
    print(ColorLogDecorator.red("Use Ctrl+C to exit."))
    ColorLogDecorator.deactivate()

    signal.signal(signal.SIGINT, stop_server)

    server.serve_forever()