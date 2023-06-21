## 这个目录主要是开发一些利于阅读源码过程中的一些工具
### 一、tool1 是一个在vim中利用gpt-3.5模型来帮助阅读源码的工具
使用教程：
1. 将tool1下的ancode.py文件放置到固定目录
2. 在.vimrc中添加配置如下：
```viml
function! Sfunc1()
	let visual_text = getreg('"')
	let command = 'python [ancode.py文件路径]' . shellescape(visual_text)
	let result = system(command)
	echo result
endfunction
`````
## Usage
1. 在vim中visual模式下复制文本
2. 在命令模式下输入:call Sfunc1()
3. 等待片刻后出现代码分析
