## SDK（4.0.4）改动说明：

1、增加跨产品升级。

2、增加连续在线15分钟后，产测不能进入功能。

3、SDK目录和编译脚本更改。详细见开发手册。

## SDK（3.1.1）改动说明：

1、更新原厂SDK2.1库。
2、提高本地定时执行准确性。
3、增加编译代码区大小。
4、解决局域网粘包问题。
5、增加AP配网不重启。
6、支持PSK2.0加密方案。
7、原厂ipv6兼容性问题合入

## 注意：

设备在AP模式配网后不重启，所以在ap配网时没有STAT_STA_UNCONN状态，如果之前固件在wifi状态回调中是根据此状态关闭配网定时器，需要做相应调整，否则导致AP配网后某些功能异常。下面为更改示例代码。

```
	...
	case STAT_STA_CONN: 
	case STAT_AP_STA_CONN:{
    	if(IsThisSysTimerRun(wf_stat_dir)){
    		sys_stop_timer(wf_stat_dir);
    	}

		if(wf_stat == STAT_STA_CONN) {
			PR_DEBUG("config_flag:%d",config_flag);
			if(config_flag == TRUE){
				config_flag = FALSE;
				reset_light_sta();
			}
			PR_DEBUG("STAT_STA_CONN");
		}
	}
	break;
	...
```