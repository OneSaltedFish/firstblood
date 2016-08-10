#include <stdio.h>
#include <string.h>
#include <malloc.h>

#include "cJSON.h"
#include "cJSON.c"

#if 0
int PLUGIN_CA_JSON_OSD(ITI_U32 uiEvent, CAINFO_OSD_t stOsdInfo, char *pcBuf)
{
    char  *pcOut  = NULL;
    cJSON *pRoot = NULL;

    if (!pcBuf)
    {
        return -1;
    }

    pRoot = cJSON_CreateObject();

    cJSON_AddItemToObject(pRoot, "frequency",    cJSON_CreateNumber(stOsdInfo.uiFre));
    cJSON_AddItemToObject(pRoot, "tsId",         cJSON_CreateNumber(stOsdInfo.usTsId));
    cJSON_AddItemToObject(pRoot, "serviceId",    cJSON_CreateNumber(stOsdInfo.usSId));
    cJSON_AddItemToObject(pRoot, "position",     cJSON_CreateString(stOsdInfo.aPosition));

    if (uiEvent == EVENT_PLUGIN_CA_OSD_DISPLAY)
    {
        cJSON_AddItemToObject(pRoot, "controlType",  cJSON_CreateString(stOsdInfo.aControlType));
        cJSON_AddItemToObject(pRoot, "count",        cJSON_CreateNumber(stOsdInfo.usCount));
        cJSON_AddItemToObject(pRoot, "timeout",      cJSON_CreateNumber(stOsdInfo.uiTimeOut));
        cJSON_AddItemToObject(pRoot, "showType",     cJSON_CreateNumber(stOsdInfo.ucShowType));
        cJSON_AddItemToObject(pRoot, "fontSize",     cJSON_CreateNumber(stOsdInfo.ucFontSize));
        cJSON_AddItemToObject(pRoot, "bgArea",       cJSON_CreateNumber(stOsdInfo.ucBgArea));
        cJSON_AddItemToObject(pRoot, "isShowCardId", cJSON_CreateNumber(stOsdInfo.ucShowCardSN));
        cJSON_AddItemToObject(pRoot, "cardid",       cJSON_CreateString(stOsdInfo.aCardid));
        cJSON_AddItemToObject(pRoot, "fontColor",    cJSON_CreateString(stOsdInfo.uiFontColor));
        cJSON_AddItemToObject(pRoot, "bgColor",      cJSON_CreateString(stOsdInfo.uiBgColor));
        cJSON_AddItemToObject(pRoot, "content",      cJSON_CreateString(stOsdInfo.aContent));
    }
    
    pcOut = cJSON_PrintUnformatted(pRoot);
    cJSON_Delete(pRoot);

    if (pcOut)
    {
        printf("pcOut:%s\n", pcOut);

        memcpy(pcBuf, pcOut, strlen(pcOut));
        free(pcOut);
        pcOut = NULL;
        return 0;
    }
    else
    {
        printf( "format json failed.\n");
        return -1;
    }
}


/* ³¬¼¶OSD¹¦ÄÜ */
void CDSTBCA_ShowOSDInfo(SCDCAOSDInfo *pOSDInfo)
{
    char  position[8] = {"bottom"};
    char  acBuf[256] = {0};
    CAINFO_OSD_t stOSDInfo;

    //printf("%s,pOSDInfo->szContent = %s\n",__FUNCTION__,pOSDInfo->szContent);
    printf("enter \n");

    memset(&stOSDInfo, 0, sizeof(CAINFO_OSD_t));
    stOSDInfo.uiFre = 0;
    stOSDInfo.usSId = 0;
    stOSDInfo.usSId = 0;

    if (pOSDInfo == NULL)
    {
        strncpy(stOSDInfo.aPosition, position, CA_OSD_POSITION_LENGH);

        PLUGIN_CA_JSON_OSD(EVENT_PLUGIN_CA_OSD_HIDE, stOSDInfo, acBuf);
        printf("[%s],%d, buf:%s\n", __FUNCTION__, __LINE__,acBuf);

        PLUGIN_CA_Event_Report(EVENT_PLUGIN_CA_OSD_HIDE, acBuf, strlen(acBuf) + 1);         
    }
    else
    {
        strncpy(stOSDInfo.aControlType, "cak", CA_OSD_CONTROLTYPE_LENGH);
        stOSDInfo.usCount       = 0;
        stOSDInfo.uiTimeOut     = 0;
        if (pOSDInfo->byDisplayMode == 0)
        {
            strncpy(stOSDInfo.aPosition, "middle", CA_OSD_POSITION_LENGH);
        }
        else if(pOSDInfo->byDisplayMode == 1)
        {
            strncpy(stOSDInfo.aPosition, "top", CA_OSD_POSITION_LENGH);
        }
        else
        {
            strncpy(stOSDInfo.aPosition, "bottom", CA_OSD_POSITION_LENGH);
        }
            
        stOSDInfo.ucShowType    = pOSDInfo->byShowType;
        stOSDInfo.ucFontSize    = pOSDInfo->byFontSize;
        stOSDInfo.ucBgArea      = pOSDInfo->byBackgroundArea;
        stOSDInfo.ucShowCardSN  = pOSDInfo->bShowCardSN;
        strncpy(stOSDInfo.aCardid, pOSDInfo->szCardSN, CA_SCSN_MAX_LEN);
        snprintf(stOSDInfo.uiFontColor, sizeof(stOSDInfo.uiFontColor), "0x%08X", (ITI_U32)pOSDInfo->dwFontColor);
        snprintf(stOSDInfo.uiBgColor, sizeof(stOSDInfo.uiFontColor), "0x%08X", (ITI_U32)pOSDInfo->dwBackgroundColor);
        strncpy(stOSDInfo.aContent, pOSDInfo->szContent, sizeof(stOSDInfo.aContent));

        PLUGIN_CA_JSON_OSD(EVENT_PLUGIN_CA_OSD_DISPLAY, stOSDInfo, acBuf);
        printf("[%s],%d, buf:%s\n", __FUNCTION__, __LINE__,acBuf);

        PLUGIN_CA_Event_Report(EVENT_PLUGIN_CA_OSD_DISPLAY, acBuf, strlen(acBuf) + 1);         
    }
}
#endif

#define CA_ENTITLE_INFO_MAX_SIZE   12 * 100
static char gEntitlesInfo[CA_ENTITLE_INFO_MAX_SIZE];
int PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo(void *pContext, int operatorId, char **ppInfo)
{
    char                       *pcOut  = NULL;
    cJSON                    *pRoot = NULL;
    cJSON                    *fmt = NULL;
    int                         iRet = -1; 
    int                         iIndex = 0;
    unsigned int       uiNumberOfObjects = 0;
    
    unsigned char         aucName[32]={0};

    printf(" operatorId:%d\n", (int)operatorId);
    
    if (!ppInfo)
    {
        return -1;
    }
    
/*	
.[0m08-14 11:00:31.673: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_CORE_getEntitlesinfo@1793   (null)] [liufei] ==============
08-14 11:00:31.673: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@695    (null)]  operatorId:43921
08-14 11:00:31.673: 760  798  DEBUG PLUGIN    [NagraJSManager_nagravision_getRequestIndex@180    (null)]  piIndex:4, enType:1668, iRet:0
08-14 11:00:31.675: 760  798  DEBUG PLUGIN    [NagraJSManager_nagravision_getRequestObject@224    (null)] type:1668, ppxObjectArray:0x741b06f4
08-14 11:00:31.675: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@704    (null)]  uiNumberOfObjects:10
08-14 11:00:31.675: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@727    (null)]  producetID:1,productName:
08-14 11:00:31.675: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@741    (null)]  startTime:2015-8-12 15:53:59
08-14 11:00:31.675: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@748    (null)]  endTime:2016-8-18 15:53:59
08-14 11:00:31.675: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@727    (null)]  producetID:2,productName:
08-14 11:00:31.676: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@741    (null)]  startTime:2015-8-12 15:58:8
08-14 11:00:31.676: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@748    (null)]  endTime:2016-8-18 15:58:8
08-14 11:00:31.676: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@727    (null)]  producetID:4,productName:
08-14 11:00:31.676: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@741    (null)]  startTime:2015-8-12 15:58:16
08-14 11:00:31.676: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@748    (null)]  endTime:2016-8-18 15:58:16
08-14 11:00:31.676: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@727    (null)]  producetID:5,productName:
08-14 11:00:31.677: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@741    (null)]  startTime:2015-8-12 15:58:16
08-14 11:00:31.677: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@748    (null)]  endTime:2016-8-18 15:58:16
08-14 11:00:31.677: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@727    (null)]  producetID:6,productName:
08-14 11:00:31.677: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@741    (null)]  startTime:2015-8-12 15:58:16
08-14 11:00:31.677: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@748    (null)]  endTime:2016-8-18 15:58:16
08-14 11:00:31.677: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@727    (null)]  producetID:7,productName:
08-14 11:00:31.678: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@741    (null)]  startTime:2015-8-12 15:58:8
08-14 11:00:31.678: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@748    (null)]  endTime:2016-8-18 15:58:8
08-14 11:00:31.678: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@727    (null)]  producetID:8,productName:
08-14 11:00:31.678: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@741    (null)]  startTime:2015-8-12 15:58:16
08-14 11:00:31.678: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@748    (null)]  endTime:2016-8-18 15:58:16
08-14 11:00:31.678: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@727    (null)]  producetID:9,productName:
08-14 11:00:31.678: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@741    (null)]  startTime:2015-8-12 15:58:16
08-14 11:00:31.679: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@748    (null)]  endTime:2016-8-18 15:58:16
08-14 11:00:32.012: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@727    (null)]  producetID:11,productName:
08-14 11:00:32.012: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@741    (null)]  startTime:2015-8-12 15:58:16
08-14 11:00:32.012: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@748    (null)]  endTime:2016-8-18 15:58:16
08-14 11:00:32.012: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@727    (null)]  producetID:12,productName:
08-14 11:00:32.013: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@741    (null)]  startTime:2015-8-12 15:58:8
08-14 11:00:32.013: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@748    (null)]  endTime:2015-8-14 10:0:0
08-14 11:00:32.014: 760  798  DEBUG PLUGIN    [PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo@760    (null)] pcOut:[{"operatorID":43921,"producetID":1,"productName":"","startTime":"2015-8-12 15:53:59","endTime":"2016-8-18 15:53:59"},{"operatorID":43921,"producetID":2,"productName":"","startTime":"2015-8-12 15:58:8","endTime":"2016-8-18 15:58:8"},{"operatorID":43921,"producetID":4,"productName":"","startTime":"2015-8-12 15:58:16","endTime":"2016-8-18 15:58:16"},{"operatorID":43921,"producetID":5,"productName":"","startTime":"2015-8-12 15:58:16","endTime":"2016-8-18 15:58:16"},{"operatorID":43921,"producetID":6,"productName":"","startTime":"2015-8-12 15:58:16","endTime":"2016-8-18 15:58:16"},{"operatorID":43921,"producetID":7,"productName":"","startTime":"2015-8-12 15:58:8","endTime":"2016-8-18 15:58:8"},{"operatorID":43921,"producetID":8,"productName":"","startTime":"2015-8-12 15:58:16","endTime":"2016-8-18 15:58:16"},{"operatorID":43921,"producetID":9,"productName":"","startTime":"2015-8-12 15:58:16","endTime":"2016-8-18 15:58:16"},{"operatorID":43921,"produc...
08-14 11:00:32.014: 760  798  DEBUG PLUGIN    [iSTB_API_PLUGIN_CA_GetEntitleInfo@415    (null)] .[34mexit ret=0
*/

    pRoot = cJSON_CreateArray();
    
	uiNumberOfObjects = 12;
    for (iIndex = 0; iIndex < (int)uiNumberOfObjects; iIndex ++)
    {
        char acTime[32] = {0};

        fmt = cJSON_CreateObject();
        cJSON_AddItemToObject(fmt, "operatorID", cJSON_CreateNumber(operatorId + iIndex));
        cJSON_AddItemToObject(fmt, "producetID", cJSON_CreateNumber(iIndex + 1));
        cJSON_AddItemToObject(fmt, "productName", cJSON_CreateString((char *)aucName));

        sprintf(acTime, "%d-%d-%d %d:%d:%d", 2015, 8, 12, 15, 58, 8 + iIndex);
        cJSON_AddItemToObject(fmt, "startTime", cJSON_CreateString(acTime));

        printf(" startTime:%s\n", acTime);
        
        memset(acTime, 0, 32);
        sprintf(acTime, "%d-%d-%d %d:%d:%d", 2016, 8, 12, 15, 58, 8 + iIndex);
        cJSON_AddItemToObject(fmt, "endTime", cJSON_CreateString(acTime));

        printf(" endTime:%s\n", acTime);
        
        cJSON_AddItemToArray(pRoot, fmt);
    }
        
    memset(gEntitlesInfo, 0, sizeof(gEntitlesInfo));

    pcOut = cJSON_PrintUnformatted(pRoot);
    cJSON_Delete(pRoot);

    if (pcOut)
    {
        printf("pcOut:%s\n", pcOut);
        //memcpy(gEntitlesInfo, pcOut, strlen(pcOut));
        memcpy(gEntitlesInfo, pcOut, sizeof(gEntitlesInfo));
		gEntitlesInfo[sizeof(gEntitlesInfo) - 1] = 0;
        free(pcOut);
        pcOut = NULL;
        *ppInfo = gEntitlesInfo;
        return 0;
    }
    else
    {
        printf("format json failed.\n");
        return -1;
    }
}

int main(int argc, char *argv0[])
{
	char *pInfo = NULL;
	char pBuf[160] = {0};

	PLUGIN_CA_NAGRA_JSON_GetEntitlesinfo(NULL, 43921, &pInfo);
	
	if (pInfo)
	printf(" len:%d, pInfo:%s\n", strlen(pInfo), pInfo);

	//PLUGIN_CA_NAGRA_JSON_ShowOSD(195, 1, 11, "top", "cak", 0xffffffff, 0xffffffff, "Hello, my name is Liufei", pBuf);
#if 0
	char *argv[5];
	int isChild = 1, canFeed = 1, feedCycle = 5;

	argv[0] = (char *)&isChild;
	argv[1] = "8000111222333";
	argv[2] = "8000111222334";
	argv[3] = (char *)&canFeed;
	argv[4] = (char *)&feedCycle;


	PLUGIN_CA_NAGRA_JSON_GetPairedStatus(3, argv, pBuf);
	if (pBuf)
	{
		printf("pInfo:%s\n", pBuf);
	}
#endif
	return 0;
}
