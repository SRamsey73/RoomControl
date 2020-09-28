def dataFromMessage(msg, tag):
    tag += ':'
    if tag in msg:
        return msg[msg.find(tag) + len(tag)]
