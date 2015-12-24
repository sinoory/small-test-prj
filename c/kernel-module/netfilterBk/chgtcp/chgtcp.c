#include <linux/init.h>
#include <linux/module.h>
#include <linux/netfilter.h>
#include <linux/socket.h>/*PF_INET*/
#include <linux/netfilter_ipv4.h>/*NF_IP_PRE_FIRST*/
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/inet.h> /*in_aton()*/
#include <net/ip.h>
#include <net/tcp.h>

#define ETHALEN 14

unsigned int watch_out(unsigned int hooknum,
                       struct sk_buff *__skb,
                       const struct net_device *in,
                       const struct net_device *out,
                       int (*okfn)(struct sk_buff *))
{
    struct sk_buff *skb;
    struct net_device *dev;
    struct iphdr *iph;
    struct tcphdr *tcph;
    int tot_len;
    int iph_len;
    int tcph_len;
    int ret;

    skb = __skb;
    if(skb == NULL)
    return NF_ACCEPT;

    iph = ip_hdr(skb);
    if(iph == NULL)
    return NF_ACCEPT;

    tot_len = ntohs(iph->tot_len);

    if(iph->daddr == in_aton("173.26.100.224"))
    {
        iph_len = ip_hdrlen(skb);/*in ip.h*/
        skb_pull(skb,iph_len);//skb->data move to tcp layer

        skb_reset_transport_header(skb);//
        if(iph->protocol == IPPROTO_TCP)
        {
            tcph = tcp_hdr(skb);
            tcph_len = tcp_hdrlen(skb);
            if(tcph->dest == htons(3306)) //
            {
                iph->saddr = in_aton("1.2.3.4");
                dev = dev_get_by_name(&init_net,"eth0");



                char *data;
                data = (char *)((int)tcph + (int)(tcph->doff * 4));

                tcph->check = 0;
                skb->csum = csum_partial((unsigned char *)tcph, tot_len - iph_len,0);
                tcph->check = csum_tcpudp_magic(iph->saddr,
                                                iph->daddr,
                                                ntohs(iph->tot_len) - iph_len,iph->protocol,
                                                skb->csum);
                iph->check = 0;
                iph->check = ip_fast_csum(iph,iph->ihl);

                skb->ip_summed = CHECKSUM_NONE;
                skb->pkt_type = PACKET_OTHERHOST;
                skb->dev = dev;
                skb_push(skb,iph_len);//
                //skb_reset_transport_header(skb);

                skb_push(skb, ETHALEN);//move skb->data to l2 , then send data via dev_queue_xmit

                ret = dev_queue_xmit(skb);
                if(ret < 0)
                {
                    printk("CHGTCP dev_queue_xmit() error\n");
                    goto out;
                }
                return NF_STOLEN;
            }
        }
        skb_push(skb,iph_len);/*在返回之前，先将skb中得信息恢复至原始L3层状态*/
        skb_reset_transport_header(skb);
    }

    return NF_ACCEPT;
out:
    dev_put(dev);
    //free(skb);

    return NF_DROP;
}



struct nf_hook_ops sample_ops = {
    .list =  {NULL,NULL},
    .hook = watch_out,
    .pf = PF_INET,
    .hooknum = NF_INET_POST_ROUTING,
    .priority = NF_IP_PRI_FIRST
};

static int __init sample_init(void) {
    nf_register_hook(&sample_ops);
    return 0;
}


static void __exit sample_exit(void) {
    nf_unregister_hook(&sample_ops);
}

module_init(sample_init);
module_exit(sample_exit); 
MODULE_AUTHOR("sin");
MODULE_DESCRIPTION("chgtcp");




