/*
 * libnsock SSL routines
 * 
 * written by Joshua J. Drake (jduck@EFNet, libnsock@qoop.org)
 */

#include "nsock.h"
#include "errors.h"

#ifdef HAVE_SSL

static int nsock_ssl_ctx_error(nsock_t *, u_int);


int
nsock_ssl_context_init(ns)
   nsock_t *ns;
{
   SSL_CTX *ctx;
   
   /* check if random stuff is initialized */
   if (!nsock_initialized)
     nsock_init();
   
   /* check out incoming struct */
   if (nsock_generic_check(ns, PF_UNSPEC, NSS_SOCKET_CONNECTED) != NSERR_SUCCESS)
     return -1;
   
   /* already initialized? */
   if (!ns->ns_ssl.context)
     {
	/* get a context depending on listen/connect */
	if (ns->backlog > 0)
	  ctx = SSL_CTX_new(SSLv23_server_method());
	else
	  ctx = SSL_CTX_new(SSLv23_client_method());
	ns->ns_ssl.context = ctx;
	if (!ctx)
	  return nsock_ssl_ctx_error(ns, NSERR_SSL_NEW_CONTEXT_FAILED);
     }

   /* set the timeout */
   SSL_CTX_set_timeout(ns->ns_ssl.context, ns->connect_timeout);
   
   /* setup the certificate file if one is set */
   if (ns->ns_ssl.cert_file)
     {
	u_char *key_file;
	
	/* is the cert usable? */
	if (!SSL_CTX_use_certificate_chain_file(ns->ns_ssl.context, 
						ns->ns_ssl.cert_file))
	  return nsock_ssl_ctx_error(ns, NSERR_SSL_INVALID_CERT);
	
	/* do we have a different key file? */
	if (ns->ns_ssl.key_file && 
	    ns->ns_ssl.key_file != ns->ns_ssl.cert_file)
	  key_file = ns->ns_ssl.key_file;
	else
	  key_file = ns->ns_ssl.cert_file;
	
	/* does it work? */
	if (!SSL_CTX_use_PrivateKey_file(ns->ns_ssl.context, key_file, 
					 SSL_FILETYPE_PEM))
	  return nsock_ssl_ctx_error(ns, NSERR_SSL_INVALID_KEY);
	
	/* does it match? */
	if (!SSL_CTX_check_private_key(ns->ns_ssl.context))
	  return nsock_ssl_ctx_error(ns, NSERR_SSL_KEY_CERT_MISMATCH);
     }
   
   /* initialzie ciphers */
   if (!SSL_CTX_set_cipher_list(ns->ns_ssl.context, SSL_DEFAULT_CIPHER_LIST))
     return nsock_ssl_ctx_error(ns, NSERR_SSL_SET_CIPHERS_FAILED);
   return NSERR_SUCCESS;
}


static int
nsock_ssl_ctx_error(ns, err)
   nsock_t *ns;
   u_int err;
{
   if (ns)
     {
	/* free it up */
	SSL_CTX_free(ns->ns_ssl.context);
	ns->ns_ssl.context = NULL;
	/* set error string */
	return nsock_error(ns, err);
     }
   return -1;
}


int
nsock_ssl_connect(ns)
   nsock_t *ns;
{
   int scr;
   
   /* check out the struct / state */
   if (nsock_generic_check(ns, PF_UNSPEC, NSS_SOCKET_CONNECTED) != NSERR_SUCCESS)
     return -1;
   
   /* make sure our context is all set */
   if (nsock_ssl_context_init(ns) != NSERR_SUCCESS)
     return -1;
   
   /* get an ssl handle */
   if (!(ns->ns_ssl.ssl = SSL_new(ns->ns_ssl.context)))
     return nsock_error(ns, NSERR_SSL_NEW_FAILED);
   
   /* set the fd */
   SSL_set_fd(ns->ns_ssl.ssl, ns->sd);
   
   /* go! */
   scr = SSL_connect(ns->ns_ssl.ssl);
   if (scr != 1)
     {
	/* found out the error type */
	ns->ns_ssl.err = SSL_get_error(ns->ns_ssl.ssl, scr);
	if (ns->ns_ssl.err == SSL_ERROR_SYSCALL)
	  return nsock_error(ns, NSERR_SSL_SYSCALL);
	if (ns->ns_ssl.err == SSL_ERROR_SSL)
	  return nsock_error(ns, NSERR_SSL_SSL);
	return nsock_error(ns, NSERR_SSL_UNKNOWN);
     }
   
   /* cool! */
   return NSERR_SUCCESS;
}


int
nsock_ssl_accept(ns)
   nsock_t *ns;
{
   int sar;
   
   /* check out the struct / state */
   if (nsock_generic_check(ns, PF_UNSPEC, NSS_SOCKET_CONNECTED) != NSERR_SUCCESS)
     return -1;
   
   /* make sure our context is all set */
   if (nsock_ssl_context_init(ns) != NSERR_SUCCESS)
     return -1;
   
   /* get an ssl handle */
   if (!(ns->ns_ssl.ssl = SSL_new(ns->ns_ssl.context)))
     return nsock_error(ns, NSERR_SSL_NEW_FAILED);
   
   /* set the fd */
   SSL_set_fd(ns->ns_ssl.ssl, ns->sd);
   
   /* get one! */
   sar = SSL_accept(ns->ns_ssl.ssl);
   if (sar != 1)
     {
	/* find out the error type */
	ns->ns_ssl.err = SSL_get_error(ns->ns_ssl.ssl, sar);
	if (ns->ns_ssl.err == SSL_ERROR_SYSCALL)
	  return nsock_error(ns, NSERR_SSL_SYSCALL);
	if (ns->ns_ssl.err == SSL_ERROR_SSL)
	  return nsock_error(ns, NSERR_SSL_SSL);
	return nsock_error(ns, NSERR_SSL_UNKNOWN);
     }
   
   /* cool! */
   return NSERR_SUCCESS;
}


#endif
