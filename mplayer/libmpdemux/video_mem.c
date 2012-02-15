typedef struct tagvideo_packs
{
	mp_mpeg_header_t picture;
	struct tagvideo_packs* next;
	int len;
	float pts;
	int flag;
	unsigned char *buf;
}video_packs_t;
char* videobuffer_mem = NULL;
static int videobuffer_mem_use = 0; 
static video_packs_t *first = NULL;
static video_packs_t *last = NULL;
static video_packs_t *current = NULL;
void FillPacks(video_packs_t *t)
{
	if(first == NULL)
	{
		last = first = t;
	}else
	{
		last->next = t;
		last = t;		
	}
	t->next = 0;
}

video_packs_t *GetPacks()
{
	if(current) free(current);
	current = NULL;
	if(first)
	{
		current = first;
		first = first->next;
	}
	return current;
}
video_packs_t *CreatePacks()
{
	video_packs_t *dp;
	dp = (video_packs_t *)malloc(sizeof(video_packs_t));
	videobuffer_mem_use = (videobuffer_mem_use & (~31));
	dp->buf = &videobuffer_mem[videobuffer_mem_use];
	//printf("create videobuffer_mem_use = %d %x\n",videobuffer_mem_use,dp->buf);
	return dp;
}
void AddPacks(video_packs_t *dp,mp_mpeg_header_t *picture,int len,float pts,int flag)
{
	memcpy(&(dp->picture),picture,sizeof(mp_mpeg_header_t));
	dp->len = len;
	dp->pts = pts;
	videobuffer_mem_use += (len + 64);
	//printf("add videobuffer_mem_use = %d len = %d\n",videobuffer_mem_use,len);
	dp->flag = flag;
	FillPacks(dp);
}
void ReleasePacks()
{
	if(videobuffer_mem == NULL) videobuffer_mem = videobuffer;
	video_packs_t *dn;
	if(current) free(current);
	current = NULL;
	dn = first;
	while(dn)
	{
		first = first->next;
		free(dn);
		dn = first;
	}
	first = NULL;
	last = NULL;
	//videobuffer_mem_use = 0;
}
