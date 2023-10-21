import secrets, string, random

def gen_str(length):
    characters = string.ascii_letters + string.digits
    return ''.join(secrets.choice(characters) for _ in range(length))

with open('inject.sql', 'w', encoding='utf-8') as f:
    raw = '''create database users;
use users;
create table userinfo (
user_id int primary key,
username varchar(10),
gender char(1),
age int,
c_id int
);
'''
    raw += 'start transaction;\n'
    raw += 'begin;\n'
    for i in range(1000000):
        user_id = i + 1
        username = gen_str(8)
        gender = '男' if random.randint(0, 2) % 1 == 0 else '女'
        age = random.randint(16, 80)
        c_id = random.randint(1, 16)
        raw += f'insert into userinfo values ({user_id}, \'{username}\', \'{gender}\', {age}, {c_id});\n'
        if i % 5000 == 0:
            f.write(raw)
            f.flush()
            raw = ''
            print(f'write {i} times')
    raw += 'commit;\n'
    f.write(raw)
    f.flush()
