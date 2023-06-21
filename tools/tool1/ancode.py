import requests
import sys

content = sys.argv[1]
print(content)

##读取文本内容，后面发送给chatgpt
#with open(file_path, "r") as file:
#    #读取内容
#    content = file.read()
#    print(content)
#
#
# 设置API访问参数
api_key = "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"  # 替换为你的OpenAI API密钥
url = "https://api.openai.com/v1/chat/completions"

# 设置请求头
headers = {
        "Content-Type": "application/json",
        "Authorization": f"Bearer {api_key}"
}

# 设置请求体
data = {
        "model": "gpt-3.5-turbo",  # 指定要使用的ChatGPT模型
    "messages": [
        { "role":"user" , "content":"帮我分析以下代码："+ content}
    ],
        "max_tokens": 1024
}


# 发送POST请求
response = requests.post(url, json=data, headers=headers)

# 解析并打印响应
response_data = response.json()
assistant_response = response_data["choices"][0]["message"]["content"]

# 打印Assistant的回复内容
print(assistant_response)
