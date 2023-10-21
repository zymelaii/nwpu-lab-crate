import secrets, string, random

def gen_str(length):
    characters = string.ascii_letters + string.digits
    return ''.join(secrets.choice(characters) for _ in range(length))

raw = 'begin;\n'
for i in range(5000):
    raw += f'insert into s values ({2005+i}, "{gen_str(4)}", "男", "2000/01/01", "CS");\n'
for i in range(1000):
    raw += f'insert into c values ({7+i}, "{gen_str(16)}", null, 2);\n'
for i in range(100000):
    raw += f'insert into sc values ({random.randint(2001, 7000)}, {random.randint(1, 1007)}, 90);\n'
raw += 'commit;\n'
with open('inject.sql', 'w', encoding='utf-8') as f:
    f.write(raw)
    f.flush()
