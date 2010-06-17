from mod_python import apache

xml_ver = "<?xml version=\"1.0\" encoding=\"UTF-8\" ?>"
xml_style = "<?xml-stylesheet href=\"style.xsl\" type=\"text/xsl\" ?>"

def anaHist(req, histselect, crate, module):
	text = xml_ver
	req.content_type = "text/html"
	req.headers_out['Pragma'] = 'no-cache'
	req.headers_out['Cache-Control'] = 'no-cache'
	req.headers_out['Expires'] = '-1'

	text += """
<html>
<head>
<title>test</tilte>
<style>
div#txt {font-size: 20pt; text-align:center;}
div#hist {float:left;}
</style>
"""
	if (req.headers_in.has_key('User-Agent')):
		text += xml_style

	if histselect == "twodpos":
		text +="""<script>
function display() {
var crateid = """
		text += str(crate);
		text += """;
"""
                text +="""document.image_source_psd.src = '../histogram/pos_2d_'+crateid+'.png';
//alert('display:2d: '+image_source_psd.src);
document.getElementById('txt').innerHTML='Two Dimensional Position Distribution at Crate# '+(crateid);
//setTimeout('parent.body.location.reload()', 50000); // 5 sec.
}
function stop_display() {
clearTimeout();
}
</script>
</head>
<body onload = 'display()' onunload='stop_display()'>
<div id='txt'></div>
<img src='' alt='busy now...' height='95%' width='95%' name='image_source_psd'></img>
</body>
</html>
"""
	elif histselect == "onedpos":
		text += """<script src='../js/daq.js' type='text/javascript'></script>
<script>
function display() {
"""
		text += "document.image_source_psd0.src = '../histogram/pos_"+str(crate)+"_"+str(module)+"_0.png';"
#		text += "alert('display:1d-pos: '+image_source_psd0.src);"
		text += "document.image_source_psd1.src = '../histogram/pos_"+str(crate)+"_"+str(module)+"_1.png';"
#		text += "alert('display:1d-pos: '+image_source_psd1.src);"
		text += "document.image_source_psd2.src = '../histogram/pos_"+str(crate)+"_"+str(module)+"_2.png';"
#		text += "alert('display:1d-pos: '+image_source_psd2.src);"
		text += "document.image_source_psd3.src = '../histogram/pos_"+str(crate)+"_"+str(module)+"_3.png';"
#		text += "alert('display:1d-pos: '+image_source_psd3.src);"
		text += "document.image_source_psd4.src = '../histogram/pos_"+str(crate)+"_"+str(module)+"_4.png';"
#		text += "alert('display:1d-pos: '+image_source_psd4.src);"
		text += "document.image_source_psd5.src = '../histogram/pos_"+str(crate)+"_"+str(module)+"_5.png';"
#		text += "alert('display:1d-pos: '+image_source_psd5.src);"
		text += "document.image_source_psd6.src = '../histogram/pos_"+str(crate)+"_"+str(module)+"_6.png';"
#		text += "alert('display:1d-pos: '+image_source_psd6.src);"
		text += "document.image_source_psd7.src = '../histogram/pos_"+str(crate)+"_"+str(module)+"_7.png';"
#		text += "alert('display:1d-pos: '+image_source_psd7.src);"
		text += "document.getElementById('txt').innerHTML='Position Distribution at VME crate# "+str(crate)+" and NEUNET# "+str(module)+"';"
	elif (histselect == "onedtof"):
		text += """<script src='../js/daq.js' type='text/javascript'></script>
<script>
function display() {
"""
		text += "document.image_source_psd0.src = '../histogram/tof_"+str(crate)+"_"+str(module)+"_0.png';"
		text += "document.image_source_psd1.src = '../histogram/tof_"+str(crate)+"_"+str(module)+"_1.png';"
		text += "document.image_source_psd2.src = '../histogram/tof_"+str(crate)+"_"+str(module)+"_2.png';"
		text += "document.image_source_psd3.src = '../histogram/tof_"+str(crate)+"_"+str(module)+"_3.png';"
		text += "document.image_source_psd4.src = '../histogram/tof_"+str(crate)+"_"+str(module)+"_4.png';"
		text += "document.image_source_psd5.src = '../histogram/tof_"+str(crate)+"_"+str(module)+"_5.png';"
		text += "document.image_source_psd6.src = '../histogram/tof_"+str(crate)+"_"+str(module)+"_6.png';"
		text += "document.image_source_psd7.src = '../histogram/tof_"+str(crate)+"_"+str(module)+"_7.png';"
		text += "document.getElementById('txt').innerHTML='TOF Distribution at VME crate# "+str(crate)+" and NEUNET# "+str(module)+"';"
	else:
		text +="<p>This is bad selection</p>"
		text +="""<br />
</body>
</html>
"""
	if histselect == "onedpos" or histselect =="onedtof" :
		text += """//setTimeout('location.reload()', 50000); // 5 sec.
}
function stop_display() {
clearTimeout();
}
</script>
</head>
<body onload = 'display()' onunload='stop_display()'>
<div id='txt'></div>
<table border='1' height='45%' width='95%'>
<tr>
  <th align='center'>PSD 0</th>
  <th align='center'>PSD 1</th>
  <th align='center'>PSD 2</th>
  <th align='center'>PSD 3</th>
</tr>
<tr valign='top'>
  <td align='left'><img src='' name='image_source_psd0'></img></td>
  <td align='left'><img src='' name='image_source_psd1'></img></td>
  <td align='left'><img src='' name='image_source_psd2'></img></td>
  <td align='left'><img src='' name='image_source_psd3'></img></td>
</tr>
</table>
<br />
<br />
<table border='1' height='45%' width='95%'>
<tr>
  <th align='center'>PSD 4</th>
  <th align='center'>PSD 5</th>
  <th align='center'>PSD 6</th>
  <th align='center'>PSD 7</th>
</tr>
<tr valign='top'>
  <td align='left'><img src='' name='image_source_psd4'></img></td>
  <td align='left'><img src='' name='image_source_psd5'></img></td>
  <td align='left'><img src='' name='image_source_psd6'></img></td>
  <td align='left'><img src='' name='image_source_psd7'></img></td>
</tr>
</table>
</body>
</html>
"""
#	print text
	req.write(text)
	return

#anaHist(1, "onedtof", 0, 0)
