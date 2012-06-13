
function unhide(divID) {
  var item = document.getElementById(divID);
  if (item) {
     item.className=(item.className=='hidden')?'unhidden':'hidden';
  }
}

function createCookie(name,value,days) {
	if (days) {
		var date = new Date();
		date.setTime(date.getTime()+(days*24*60*60*1000));
		var expires = "; expires="+date.toGMTString();
	}
	else var expires = "";
	document.cookie = name+"="+value+expires+"; path=/";
}

function readCookie(name) {
	var nameEQ = name + "=";
	var ca = document.cookie.split(';');
	for(var i=0;i < ca.length;i++) {
		var c = ca[i];
		while (c.charAt(0)==' ') c = c.substring(1,c.length);
		if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length,c.length);
	}
	return null;
}

function eraseCookie(name) {
	createCookie(name,"",-1);
}


function changeImageURLs() {
   var remoteURL = "http://www-d0.fnal.gov/~dsmirnov/preons/";
   var imgs,i, imglinks;
   docBasePath = document.location.href;
   len = docBasePath.length;
   last = docBasePath.lastIndexOf("/");

   imgs=document.getElementsByTagName('img');

   for(i in imgs) {
      if(/remoteurl1/.test(imgs[i].className)) {
        totlen = imgs[i].src.length;
        imgs[i].src = remoteURL + imgs[i].src.substring(last, totlen);
        //alert(docBasePath + "");
        //alert(imgs[i].src.substring(last, totlen));
      }
   }

   imglinks = document.getElementsByTagName('a');
   for(i in imglinks) {
      if(/remoteurl1/.test(imglinks[i].className)) {
        totlen = imglinks[i].href.length;
        imglinks[i].href = remoteURL + imglinks[i].href.substring(last, totlen);
        //alert(imglinks[i].href);
      }
   }
}
