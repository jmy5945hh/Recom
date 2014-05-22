# -*- coding: gb18030 -*-
import urllib2 as ur
import urllib
import socket
from urllib import urlencode
import time,re,sys,os,time,Queue,json

#jiayuan配置信息
uid = '100774534'
passwd = '99912345sj'

#组件信息
proxy_handler = ur.ProxyHandler({'http': 'http://127.0.0.1:8080/'})
urlopener = ur.build_opener(ur.HTTPCookieProcessor())

#http头
header={'User-Agent':'Mozilla/5.0 (Windows NT 6.1; WOW64) AppleWebKit/537.31 (KHTML, like Gecko) Chrome/26.0.1410.64 Safari/537.31',
         'Accept':'text/html,application/xhtml+xml,application/xml;q=0.9,*/*;q=0.8',
         'Connection': 'keep-alive'
         }
searchhead={'User-Agent':'Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30)',
           'Referer':'http://search.jiayuan.com/',
           'Origin':'http://search.jiayuan.com',
            'Connection': 'keep-alive'}

searchhead1={'User-Agent':'Mozilla/4.0 (compatible; MSIE 8.0; Windows NT 5.1; Trident/4.0; .NET CLR 2.0.50727; .NET CLR 3.0.04506.30)',
           'Referer':'http://search.jiayuan.com/index.php?y=0&re=1&p=1&keepcond&t=10',
             'Connection': 'keep-alive'}


#用户登入数据
values = {'password':passwd,'name':uid}

#搜索条件
search_values = {'sex':'f',
                 'min_age':19,
                 'max_age':24,
                 'work_location':51,
                 'work_sublocation':5101,
                 'education':30,
                 'edu_more_than':'on',
                 'avatar':1,
                 'marrige':1,
                 'income':20,
                 'income_more_than':'on'
                 #'level':3
                 }


#post数据转换程序
def data(values):
    data=urlencode(values).encode()
    return data


#访问指定页面
def geturlcon(url,data=None,header=header,byte=None):
    request = ur.Request(url,data,header)
    url = urlopener.open(request)
    page=url.read(byte).decode('utf8','ignore')
    return page

#获得搜索用户信息
def getsearchinfo():
    content = search()
    
    pattern = re.compile(r'(?<=resultCount\s\s\s=\s)\d+')
    count = pattern.search(content)
    print u'总共搜索到', int(count.group())-5,u'个用户'
    totalpage = (int(count.group())-5)/30+1
    print u'需要处理',totalpage,u'页'
    
    usercount = 1
    #翻页
    a = 1
    
    while(totalpage):
        page_value = {'p':a}
        print u'正在处理第', a, u'页...'
        url11='http://search.jiayuan.com/index.php?y=0&re=1&p=1&keepcond&t=10'
        page=geturlcon(url11,data(page_value),header)
        content = page.replace('\\/', '/')
        normal_user = re.findall('var\s*normal_user\s*=\s*(\[[^\]]+\])', content)[0]
        normal_user.decode('gbk','ignore').encode('utf-8')
        normal_user = re.sub('(?<=<img\\ssrc=)"(http://[^"]*)[^>]*', "\\1 ", normal_user)
        normal_user = normal_user.decode('gbk', 'ignore')
        try:
            normal_user = json.loads(normal_user)
        except ValueError:
            a=a+1
            totalpage = totalpage - 1
            print 'ValueError'
            ef = file('errorpage.txt', 'a')
            ef.write(normal_user)
            continue;
            
        for item in normal_user:
            ff = file('data.txt', 'a')
            ff.write(str(usercount));ff.write(' ')
            ff.write(item['uid']);ff.write('    ')
            ff.write(item['nickname'].encode('gb18030'));ff.write(' ')
            ff.write(item['work_location'].encode('gbk'));ff.write('    ')
            ff.write(str(item['age']));ff.write('   ')
            ff.write(item['height']);ff.write(' ')
            ff.write(item['income'].encode('gb18030'));ff.write('   ')
            ff.write(item['education'].encode('gbk'));ff.write('    ')
            ff.write(item['industry'].encode('gbk'));ff.write(' ')
            ff.write(item['astro'].encode('gbk'))
            
            ff.write('\n')
            fname = item['uid'] + '.jpg'
            try:
                socket.setdefaulttimeout(5)
                urllib.urlretrieve(item['avatar'], fname)
            except IOError:
                print 'IOError' + item['avatar']
            usercount = usercount+1

        a=a+1
        totalpage = totalpage - 1


#检测是否登入成功
def checklogin(page):
    if page.find(uid)>0:
        return True
    else:
        return False

#访问登陆页面（获得cookie）
def login():
    print 'uid：',uid,u'登录中,请稍等……'
    url1='http://login.jiayuan.com/dologin.php'
    page=geturlcon(url1,data(values),header)
    url2='http://www.jiayuan.com/usercp/'  
    checkpage=geturlcon(url2)  
    return checklogin(checkpage)

#访问搜索页面
def search():
    url3='http://search.jiayuan.com/result.php?t=10&m=1'
    page=geturlcon(url3,data(search_values),header)
    return page
       
if __name__ == '__main__':
    if login():
        print '登录成功！'
        getsearchinfo()
    else:
        print '登录失败，正在重试……'
        login()
