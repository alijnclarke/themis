/**
 * @file
 *
 * (c) CossackLabs
 */

#include "common/error.h"
#include "soter/soter.h"
#include "soter_openssl.h"
#include <openssl/evp.h>


static const EVP_MD* soter_algo_to_evp_md(soter_hash_algo_t algo)
{
	switch (algo)
	{
	case SOTER_HASH_SHA1:
		return EVP_sha1();
	case SOTER_HASH_SHA256:
	  return EVP_sha256();
	case SOTER_HASH_SHA512:
		return EVP_sha512();
	default:
		return NULL;
	}
}

soter_status_t soter_hash_init(soter_hash_ctx_t *hash_ctx, soter_hash_algo_t algo)
{
	const EVP_MD *md = soter_algo_to_evp_md(algo);

	if (!hash_ctx || !md)
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (EVP_DigestInit(&(hash_ctx->evp_md_ctx), md))
	{
		return HERMES_SUCCESS;
	}
	else
	{
		return HERMES_FAIL;
	}
}

soter_status_t soter_hash_update(soter_hash_ctx_t *hash_ctx, const void *data, size_t length)
{
	if (!hash_ctx || !data)
	{
		return HERMES_INVALID_PARAMETER;
	}

	if (EVP_DigestUpdate(&(hash_ctx->evp_md_ctx), data, length))
	{
		return HERMES_SUCCESS;
	}
	else
	{
		return HERMES_FAIL;
	}
}

soter_status_t soter_hash_final(soter_hash_ctx_t *hash_ctx, uint8_t* hash_value, size_t* hash_length)
{
	size_t md_length;

	if (!hash_ctx || !hash_length)
	{
		return HERMES_INVALID_PARAMETER;
	}

	md_length = (size_t)EVP_MD_CTX_size(&(hash_ctx->evp_md_ctx));

	if (!hash_value || (md_length > *hash_length))
	{
		*hash_length = md_length;
		return HERMES_BUFFER_TOO_SMALL;
	}

	if (EVP_DigestFinal(&(hash_ctx->evp_md_ctx), hash_value, (unsigned int *)&md_length))
	{
		*hash_length = md_length;
		return HERMES_SUCCESS;
	}
	else
	{
		return HERMES_FAIL;
	}
}

soter_hash_ctx_t* soter_hash_create(soter_hash_algo_t algo)
{
	soter_status_t status;
	soter_hash_ctx_t *ctx = malloc(sizeof(soter_hash_ctx_t));
	if (!ctx)
	{
		return NULL;
	}

	status = soter_hash_init(ctx, algo);
	if (HERMES_SUCCESS == status)
	{
		return ctx;
	}
	else
	{
		free(ctx);
		return NULL;
	}
}

soter_status_t soter_hash_destroy(soter_hash_ctx_t *hash_ctx)
{
	if (!hash_ctx)
	{
		return HERMES_INVALID_PARAMETER;
	}

	free(hash_ctx);
	return HERMES_SUCCESS;
}