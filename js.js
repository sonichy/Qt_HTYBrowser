var imgs=document.getElementsByTagName('img');
for(i=0;i<imgs.length;i++){
if(imgs[i].src.indexOf('http://ads.')!=-1 || imgs[i].src.indexOf('://ww2.sinaimg.cn')!=-1 || imgs[i].src.indexOf('://ww3.sinaimg.cn')!=-1 || imgs[i].src.indexOf('http://vip.xinzheng8.pw')!=-1 || imgs[i].src.indexOf('://wx3.sinaimg.cn')!=-1 || imgs[i].src.indexOf('://wx4.sinaimg.cn')!=-1 || imgs[i].src.indexOf('http://www.pv84.com')!=-1 || imgs[i].src.indexOf('://img.9118ads.com')!=-1 || imgs[i].src.indexOf('://wwwcdn.4006578517.com')!=-1 ) 		
	imgs[i].style.display='none';
}