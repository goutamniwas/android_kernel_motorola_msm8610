/*
 * fs/f2fs/file.c
 *
 * Copyright (c) 2012 Samsung Electronics Co., Ltd.
 *             http://www.samsung.com/
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */
#include <linux/fs.h>
#include <linux/f2fs_fs.h>
#include <linux/stat.h>
#include <linux/buffer_head.h>
#include <linux/writeback.h>
#include <linux/blkdev.h>
#include <linux/falloc.h>
#include <linux/types.h>
#include <linux/compat.h>
#include <linux/uaccess.h>
#include <linux/mount.h>
<<<<<<< HEAD
#include <linux/pagevec.h>
=======
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4

#include "f2fs.h"
#include "node.h"
#include "segment.h"
#include "xattr.h"
#include "acl.h"
#include <trace/events/f2fs.h>

static int f2fs_vm_page_mkwrite(struct vm_area_struct *vma,
						struct vm_fault *vmf)
{
	struct page *page = vmf->page;
<<<<<<< HEAD
	struct inode *inode = file_inode(vma->vm_file);
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
=======
	struct inode *inode = vma->vm_file->f_path.dentry->d_inode;
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	struct dnode_of_data dn;
	int err;

	f2fs_balance_fs(sbi);

<<<<<<< HEAD
	vfs_check_frozen(inode->i_sb, SB_FREEZE_WRITE);

	f2fs_bug_on(sbi, f2fs_has_inline_data(inode));

=======
	/* Wait if fs is frozen. This is racy so we check again later on
	 * and retry if the fs has been frozen after the page lock has
	 * been acquired
	 */
	vfs_check_frozen(inode->i_sb, SB_FREEZE_WRITE);

>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	/* block allocation */
	f2fs_lock_op(sbi);
	set_new_dnode(&dn, inode, NULL, NULL, 0);
	err = f2fs_reserve_block(&dn, page->index);
<<<<<<< HEAD
	if (err) {
		f2fs_unlock_op(sbi);
		goto out;
	}
	f2fs_put_dnode(&dn);
	f2fs_unlock_op(sbi);
=======
	f2fs_unlock_op(sbi);
	if (err)
		goto out;
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4

	file_update_time(vma->vm_file);
	lock_page(page);
	if (unlikely(page->mapping != inode->i_mapping ||
			page_offset(page) > i_size_read(inode) ||
			!PageUptodate(page))) {
		unlock_page(page);
		err = -EFAULT;
		goto out;
	}

	/*
	 * check to see if the page is mapped already (no holes)
	 */
	if (PageMappedToDisk(page))
		goto mapped;

	/* page is wholly or partially inside EOF */
	if (((page->index + 1) << PAGE_CACHE_SHIFT) > i_size_read(inode)) {
		unsigned offset;
		offset = i_size_read(inode) & ~PAGE_CACHE_MASK;
		zero_user_segment(page, offset, PAGE_CACHE_SIZE);
	}
	set_page_dirty(page);
	SetPageUptodate(page);

	trace_f2fs_vm_page_mkwrite(page, DATA);
mapped:
	/* fill the page */
<<<<<<< HEAD
	f2fs_wait_on_page_writeback(page, DATA);
=======
	wait_on_page_writeback(page);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
out:
	return block_page_mkwrite_return(err);
}

static const struct vm_operations_struct f2fs_file_vm_ops = {
<<<<<<< HEAD
	.fault		= filemap_fault,
	.page_mkwrite	= f2fs_vm_page_mkwrite,
=======
	.fault        = filemap_fault,
	.page_mkwrite = f2fs_vm_page_mkwrite,
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
};

static int get_parent_ino(struct inode *inode, nid_t *pino)
{
	struct dentry *dentry;

	inode = igrab(inode);
	dentry = d_find_any_alias(inode);
	iput(inode);
	if (!dentry)
		return 0;

	if (update_dent_inode(inode, &dentry->d_name)) {
		dput(dentry);
		return 0;
	}

	*pino = parent_ino(dentry);
	dput(dentry);
	return 1;
}

<<<<<<< HEAD
static inline bool need_do_checkpoint(struct inode *inode)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	bool need_cp = false;

	if (!S_ISREG(inode->i_mode) || inode->i_nlink != 1)
		need_cp = true;
	else if (file_wrong_pino(inode))
		need_cp = true;
	else if (!space_for_roll_forward(sbi))
		need_cp = true;
	else if (!is_checkpointed_node(sbi, F2FS_I(inode)->i_pino))
		need_cp = true;
	else if (F2FS_I(inode)->xattr_ver == cur_cp_version(F2FS_CKPT(sbi)))
		need_cp = true;

	return need_cp;
}

int f2fs_sync_file(struct file *file, loff_t start, loff_t end, int datasync)
{
	struct inode *inode = file->f_mapping->host;
	struct f2fs_inode_info *fi = F2FS_I(inode);
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
	nid_t ino = inode->i_ino;
	int ret = 0;
	bool need_cp = false;
	struct writeback_control wbc = {
		.sync_mode = WB_SYNC_ALL,
=======
int f2fs_sync_file(struct file *file, loff_t start, loff_t end, int datasync)
{
	struct inode *inode = file->f_mapping->host;
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
	int ret = 0;
	bool need_cp = false;
	struct writeback_control wbc = {
		.sync_mode = WB_SYNC_NONE,
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
		.nr_to_write = LONG_MAX,
		.for_reclaim = 0,
	};

	if (unlikely(f2fs_readonly(inode->i_sb)))
		return 0;

	trace_f2fs_sync_file_enter(inode);
<<<<<<< HEAD

	/* if fdatasync is triggered, let's do in-place-update */
	if (get_dirty_pages(inode) <= SM_I(sbi)->min_fsync_blocks)
		set_inode_flag(fi, FI_NEED_IPU);
	ret = filemap_write_and_wait_range(inode->i_mapping, start, end);
	clear_inode_flag(fi, FI_NEED_IPU);

=======
	ret = filemap_write_and_wait_range(inode->i_mapping, start, end);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	if (ret) {
		trace_f2fs_sync_file_exit(inode, need_cp, datasync, ret);
		return ret;
	}

<<<<<<< HEAD
	/*
	 * if there is no written data, don't waste time to write recovery info.
	 */
	if (!is_inode_flag_set(fi, FI_APPEND_WRITE) &&
			!exist_written_data(sbi, ino, APPEND_INO)) {
		struct page *i = find_get_page(NODE_MAPPING(sbi), ino);

		/* But we need to avoid that there are some inode updates */
		if ((i && PageDirty(i)) || need_inode_block_update(sbi, ino)) {
			f2fs_put_page(i, 0);
			goto go_write;
		}
		f2fs_put_page(i, 0);

		if (is_inode_flag_set(fi, FI_UPDATE_WRITE) ||
				exist_written_data(sbi, ino, UPDATE_INO))
			goto flush_out;
		goto out;
	}
go_write:
	/* guarantee free sections for fsync */
	f2fs_balance_fs(sbi);

=======
	/* guarantee free sections for fsync */
	f2fs_balance_fs(sbi);

	mutex_lock(&inode->i_mutex);

>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	/*
	 * Both of fdatasync() and fsync() are able to be recovered from
	 * sudden-power-off.
	 */
<<<<<<< HEAD
	down_read(&fi->i_sem);
	need_cp = need_do_checkpoint(inode);
	up_read(&fi->i_sem);
=======
	if (!S_ISREG(inode->i_mode) || inode->i_nlink != 1)
		need_cp = true;
	else if (file_wrong_pino(inode))
		need_cp = true;
	else if (!space_for_roll_forward(sbi))
		need_cp = true;
	else if (!is_checkpointed_node(sbi, F2FS_I(inode)->i_pino))
		need_cp = true;
	else if (F2FS_I(inode)->xattr_ver == cur_cp_version(F2FS_CKPT(sbi)))
		need_cp = true;
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4

	if (need_cp) {
		nid_t pino;

<<<<<<< HEAD
		/* all the dirty node pages should be flushed for POR */
		ret = f2fs_sync_fs(inode->i_sb, 1);

		down_write(&fi->i_sem);
		F2FS_I(inode)->xattr_ver = 0;
=======
		F2FS_I(inode)->xattr_ver = 0;

		/* all the dirty node pages should be flushed for POR */
		ret = f2fs_sync_fs(inode->i_sb, 1);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
		if (file_wrong_pino(inode) && inode->i_nlink == 1 &&
					get_parent_ino(inode, &pino)) {
			F2FS_I(inode)->i_pino = pino;
			file_got_pino(inode);
<<<<<<< HEAD
			up_write(&fi->i_sem);
=======
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
			mark_inode_dirty_sync(inode);
			ret = f2fs_write_inode(inode, NULL);
			if (ret)
				goto out;
<<<<<<< HEAD
		} else {
			up_write(&fi->i_sem);
		}
	} else {
sync_nodes:
		sync_node_pages(sbi, ino, &wbc);

		if (need_inode_block_update(sbi, ino)) {
=======
		}
	} else {
		/* if there is no written node page, write its inode page */
		while (!sync_node_pages(sbi, inode->i_ino, &wbc)) {
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
			mark_inode_dirty_sync(inode);
			ret = f2fs_write_inode(inode, NULL);
			if (ret)
				goto out;
<<<<<<< HEAD
			goto sync_nodes;
		}

		ret = wait_on_node_pages_writeback(sbi, ino);
		if (ret)
			goto out;

		/* once recovery info is written, don't need to tack this */
		remove_dirty_inode(sbi, ino, APPEND_INO);
		clear_inode_flag(fi, FI_APPEND_WRITE);
flush_out:
		remove_dirty_inode(sbi, ino, UPDATE_INO);
		clear_inode_flag(fi, FI_UPDATE_WRITE);
		ret = f2fs_issue_flush(F2FS_I_SB(inode));
	}
out:
=======
		}
		ret = wait_on_node_pages_writeback(sbi, inode->i_ino);
		if (ret)
			goto out;
		ret = blkdev_issue_flush(inode->i_sb->s_bdev, GFP_KERNEL, NULL);
	}
out:
	mutex_unlock(&inode->i_mutex);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	trace_f2fs_sync_file_exit(inode, need_cp, datasync, ret);
	return ret;
}

<<<<<<< HEAD
static pgoff_t __get_first_dirty_index(struct address_space *mapping,
						pgoff_t pgofs, int whence)
{
	struct pagevec pvec;
	int nr_pages;

	if (whence != SEEK_DATA)
		return 0;

	/* find first dirty page index */
	pagevec_init(&pvec, 0);
	nr_pages = pagevec_lookup_tag(&pvec, mapping, &pgofs,
					PAGECACHE_TAG_DIRTY, 1);
	pgofs = nr_pages ? pvec.pages[0]->index : LONG_MAX;
	pagevec_release(&pvec);
	return pgofs;
}

static bool __found_offset(block_t blkaddr, pgoff_t dirty, pgoff_t pgofs,
							int whence)
{
	switch (whence) {
	case SEEK_DATA:
		if ((blkaddr == NEW_ADDR && dirty == pgofs) ||
			(blkaddr != NEW_ADDR && blkaddr != NULL_ADDR))
			return true;
		break;
	case SEEK_HOLE:
		if (blkaddr == NULL_ADDR)
			return true;
		break;
	}
	return false;
}

static inline int unsigned_offsets(struct file *file)
{
	return file->f_mode & FMODE_UNSIGNED_OFFSET;
}

static loff_t vfs_setpos(struct file *file, loff_t offset, loff_t maxsize)
{
	if (offset < 0 && !unsigned_offsets(file))
		return -EINVAL;
	if (offset > maxsize)
		return -EINVAL;

	if (offset != file->f_pos) {
		file->f_pos = offset;
		file->f_version = 0;
	}
	return offset;
}

static loff_t f2fs_seek_block(struct file *file, loff_t offset, int whence)
{
	struct inode *inode = file->f_mapping->host;
	loff_t maxbytes = inode->i_sb->s_maxbytes;
	struct dnode_of_data dn;
	pgoff_t pgofs, end_offset, dirty;
	loff_t data_ofs = offset;
	loff_t isize;
	int err = 0;

	mutex_lock(&inode->i_mutex);

	isize = i_size_read(inode);
	if (offset >= isize)
		goto fail;

	/* handle inline data case */
	if (f2fs_has_inline_data(inode) || f2fs_has_inline_dentry(inode)) {
		if (whence == SEEK_HOLE)
			data_ofs = isize;
		goto found;
	}

	pgofs = (pgoff_t)(offset >> PAGE_CACHE_SHIFT);

	dirty = __get_first_dirty_index(inode->i_mapping, pgofs, whence);

	for (; data_ofs < isize; data_ofs = pgofs << PAGE_CACHE_SHIFT) {
		set_new_dnode(&dn, inode, NULL, NULL, 0);
		err = get_dnode_of_data(&dn, pgofs, LOOKUP_NODE_RA);
		if (err && err != -ENOENT) {
			goto fail;
		} else if (err == -ENOENT) {
			/* direct node does not exists */
			if (whence == SEEK_DATA) {
				pgofs = PGOFS_OF_NEXT_DNODE(pgofs,
							F2FS_I(inode));
				continue;
			} else {
				goto found;
			}
		}

		end_offset = ADDRS_PER_PAGE(dn.node_page, F2FS_I(inode));

		/* find data/hole in dnode block */
		for (; dn.ofs_in_node < end_offset;
				dn.ofs_in_node++, pgofs++,
				data_ofs = pgofs << PAGE_CACHE_SHIFT) {
			block_t blkaddr;
			blkaddr = datablock_addr(dn.node_page, dn.ofs_in_node);

			if (__found_offset(blkaddr, dirty, pgofs, whence)) {
				f2fs_put_dnode(&dn);
				goto found;
			}
		}
		f2fs_put_dnode(&dn);
	}

	if (whence == SEEK_DATA)
		goto fail;
found:
	if (whence == SEEK_HOLE && data_ofs > isize)
		data_ofs = isize;
	mutex_unlock(&inode->i_mutex);
	return vfs_setpos(file, data_ofs, maxbytes);
fail:
	mutex_unlock(&inode->i_mutex);
	return -ENXIO;
}

static loff_t f2fs_llseek(struct file *file, loff_t offset, int whence)
{
	struct inode *inode = file->f_mapping->host;
	loff_t maxbytes = inode->i_sb->s_maxbytes;

	switch (whence) {
	case SEEK_SET:
	case SEEK_CUR:
	case SEEK_END:
		return generic_file_llseek_size(file, offset, whence,
						maxbytes);
	case SEEK_DATA:
	case SEEK_HOLE:
		if (offset < 0)
			return -ENXIO;
		return f2fs_seek_block(file, offset, whence);
	}

	return -EINVAL;
}

static int f2fs_file_mmap(struct file *file, struct vm_area_struct *vma)
{
	struct inode *inode = file_inode(file);

	/* we don't need to use inline_data strictly */
	if (f2fs_has_inline_data(inode)) {
		int err = f2fs_convert_inline_inode(inode);
		if (err)
			return err;
	}

=======
static int f2fs_file_mmap(struct file *file, struct vm_area_struct *vma)
{
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	file_accessed(file);
	vma->vm_ops = &f2fs_file_vm_ops;
	return 0;
}

int truncate_data_blocks_range(struct dnode_of_data *dn, int count)
{
	int nr_free = 0, ofs = dn->ofs_in_node;
<<<<<<< HEAD
	struct f2fs_sb_info *sbi = F2FS_I_SB(dn->inode);
=======
	struct f2fs_sb_info *sbi = F2FS_SB(dn->inode->i_sb);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	struct f2fs_node *raw_node;
	__le32 *addr;

	raw_node = F2FS_NODE(dn->node_page);
	addr = blkaddr_in_node(raw_node) + ofs;

	for (; count > 0; count--, addr++, dn->ofs_in_node++) {
		block_t blkaddr = le32_to_cpu(*addr);
		if (blkaddr == NULL_ADDR)
			continue;

		update_extent_cache(NULL_ADDR, dn);
		invalidate_blocks(sbi, blkaddr);
		nr_free++;
	}
	if (nr_free) {
		dec_valid_block_count(sbi, dn->inode, nr_free);
		set_page_dirty(dn->node_page);
		sync_inode_page(dn);
	}
	dn->ofs_in_node = ofs;

	trace_f2fs_truncate_data_blocks_range(dn->inode, dn->nid,
					 dn->ofs_in_node, nr_free);
	return nr_free;
}

void truncate_data_blocks(struct dnode_of_data *dn)
{
	truncate_data_blocks_range(dn, ADDRS_PER_BLOCK);
}

<<<<<<< HEAD
static int truncate_partial_data_page(struct inode *inode, u64 from)
=======
static void truncate_partial_data_page(struct inode *inode, u64 from)
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
{
	unsigned offset = from & (PAGE_CACHE_SIZE - 1);
	struct page *page;

	if (!offset)
<<<<<<< HEAD
		return 0;

	page = find_data_page(inode, from >> PAGE_CACHE_SHIFT, false);
	if (IS_ERR(page))
		return 0;

	lock_page(page);
	if (unlikely(!PageUptodate(page) ||
			page->mapping != inode->i_mapping))
		goto out;

	f2fs_wait_on_page_writeback(page, DATA);
	zero_user(page, offset, PAGE_CACHE_SIZE - offset);
	set_page_dirty(page);
out:
	f2fs_put_page(page, 1);
	return 0;
}

int truncate_blocks(struct inode *inode, u64 from, bool lock)
{
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
=======
		return;

	page = find_data_page(inode, from >> PAGE_CACHE_SHIFT, false);
	if (IS_ERR(page))
		return;

	lock_page(page);
	if (unlikely(page->mapping != inode->i_mapping)) {
		f2fs_put_page(page, 1);
		return;
	}
	wait_on_page_writeback(page);
	zero_user(page, offset, PAGE_CACHE_SIZE - offset);
	set_page_dirty(page);
	f2fs_put_page(page, 1);
}

int truncate_blocks(struct inode *inode, u64 from)
{
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	unsigned int blocksize = inode->i_sb->s_blocksize;
	struct dnode_of_data dn;
	pgoff_t free_from;
	int count = 0, err = 0;
<<<<<<< HEAD
	struct page *ipage;

	trace_f2fs_truncate_blocks_enter(inode, from);

	free_from = (pgoff_t)
		((from + blocksize - 1) >> (sbi->log_blocksize));

	if (lock)
		f2fs_lock_op(sbi);

	ipage = get_node_page(sbi, inode->i_ino);
	if (IS_ERR(ipage)) {
		err = PTR_ERR(ipage);
		goto out;
	}

	if (f2fs_has_inline_data(inode)) {
		f2fs_put_page(ipage, 1);
		goto out;
	}

	set_new_dnode(&dn, inode, ipage, NULL, 0);
=======

	trace_f2fs_truncate_blocks_enter(inode, from);

	if (f2fs_has_inline_data(inode))
		goto done;

	free_from = (pgoff_t)
			((from + blocksize - 1) >> (sbi->log_blocksize));

	f2fs_lock_op(sbi);

	set_new_dnode(&dn, inode, NULL, NULL, 0);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	err = get_dnode_of_data(&dn, free_from, LOOKUP_NODE);
	if (err) {
		if (err == -ENOENT)
			goto free_next;
<<<<<<< HEAD
		goto out;
	}

	count = ADDRS_PER_PAGE(dn.node_page, F2FS_I(inode));

	count -= dn.ofs_in_node;
	f2fs_bug_on(sbi, count < 0);
=======
		f2fs_unlock_op(sbi);
		trace_f2fs_truncate_blocks_exit(inode, err);
		return err;
	}

	if (IS_INODE(dn.node_page))
		count = ADDRS_PER_INODE(F2FS_I(inode));
	else
		count = ADDRS_PER_BLOCK;

	count -= dn.ofs_in_node;
	f2fs_bug_on(count < 0);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4

	if (dn.ofs_in_node || IS_INODE(dn.node_page)) {
		truncate_data_blocks_range(&dn, count);
		free_from += count;
	}

	f2fs_put_dnode(&dn);
free_next:
	err = truncate_inode_blocks(inode, free_from);
<<<<<<< HEAD
out:
	if (lock)
		f2fs_unlock_op(sbi);

	/* lastly zero out the first data page */
	if (!err)
		err = truncate_partial_data_page(inode, from);
=======
	f2fs_unlock_op(sbi);
done:
	/* lastly zero out the first data page */
	truncate_partial_data_page(inode, from);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4

	trace_f2fs_truncate_blocks_exit(inode, err);
	return err;
}

void f2fs_truncate(struct inode *inode)
{
<<<<<<< HEAD
=======
	int err;

>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	if (!(S_ISREG(inode->i_mode) || S_ISDIR(inode->i_mode) ||
				S_ISLNK(inode->i_mode)))
		return;

	trace_f2fs_truncate(inode);

<<<<<<< HEAD
	/* we should check inline_data size */
	if (f2fs_has_inline_data(inode) && !f2fs_may_inline(inode)) {
		if (f2fs_convert_inline_inode(inode))
			return;
	}

	if (!truncate_blocks(inode, i_size_read(inode), true)) {
=======
	err = truncate_blocks(inode, i_size_read(inode));
	if (err) {
		f2fs_msg(inode->i_sb, KERN_ERR, "truncate failed with %d",
				err);
		f2fs_handle_error(F2FS_SB(inode->i_sb));
	} else {
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
		inode->i_mtime = inode->i_ctime = CURRENT_TIME;
		mark_inode_dirty(inode);
	}
}

int f2fs_getattr(struct vfsmount *mnt,
			 struct dentry *dentry, struct kstat *stat)
{
	struct inode *inode = dentry->d_inode;
	generic_fillattr(inode, stat);
	stat->blocks <<= 3;
	return 0;
}

#ifdef CONFIG_F2FS_FS_POSIX_ACL
static void __setattr_copy(struct inode *inode, const struct iattr *attr)
{
	struct f2fs_inode_info *fi = F2FS_I(inode);
	unsigned int ia_valid = attr->ia_valid;

	if (ia_valid & ATTR_UID)
		inode->i_uid = attr->ia_uid;
	if (ia_valid & ATTR_GID)
		inode->i_gid = attr->ia_gid;
	if (ia_valid & ATTR_ATIME)
		inode->i_atime = timespec_trunc(attr->ia_atime,
						inode->i_sb->s_time_gran);
	if (ia_valid & ATTR_MTIME)
		inode->i_mtime = timespec_trunc(attr->ia_mtime,
						inode->i_sb->s_time_gran);
	if (ia_valid & ATTR_CTIME)
		inode->i_ctime = timespec_trunc(attr->ia_ctime,
						inode->i_sb->s_time_gran);
	if (ia_valid & ATTR_MODE) {
		umode_t mode = attr->ia_mode;

		if (!in_group_p(inode->i_gid) && !capable(CAP_FSETID))
			mode &= ~S_ISGID;
		set_acl_inode(fi, mode);
	}
}
#else
#define __setattr_copy setattr_copy
#endif

int f2fs_setattr(struct dentry *dentry, struct iattr *attr)
{
	struct inode *inode = dentry->d_inode;
	struct f2fs_inode_info *fi = F2FS_I(inode);
<<<<<<< HEAD
=======
	struct f2fs_inode_info *pfi = F2FS_I(dentry->d_parent->d_inode);
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	int err;

	err = inode_change_ok(inode, attr);
	if (err)
		return err;

<<<<<<< HEAD
	if (attr->ia_valid & ATTR_SIZE) {
		if (attr->ia_size != i_size_read(inode)) {
			truncate_setsize(inode, attr->ia_size);
			f2fs_truncate(inode);
			f2fs_balance_fs(F2FS_I_SB(inode));
		} else {
			/*
			 * giving a chance to truncate blocks past EOF which
			 * are fallocated with FALLOC_FL_KEEP_SIZE.
			 */
			f2fs_truncate(inode);
		}
=======
	if (IS_ANDROID_EMU(sbi, fi, pfi))
		f2fs_android_emu(sbi, inode, &attr->ia_uid, &attr->ia_gid,
				 &attr->ia_mode);

	if ((attr->ia_valid & ATTR_SIZE) &&
			attr->ia_size != i_size_read(inode)) {
		err = f2fs_convert_inline_data(inode, attr->ia_size);
		if (err)
			return err;

		truncate_setsize(inode, attr->ia_size);
		f2fs_truncate(inode);
		f2fs_balance_fs(F2FS_SB(inode->i_sb));
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	}

	__setattr_copy(inode, attr);

	if (attr->ia_valid & ATTR_MODE) {
		err = f2fs_acl_chmod(inode);
		if (err || is_inode_flag_set(fi, FI_ACL_MODE)) {
			inode->i_mode = fi->i_acl_mode;
			clear_inode_flag(fi, FI_ACL_MODE);
		}
	}

	mark_inode_dirty(inode);
	return err;
}

const struct inode_operations f2fs_file_inode_operations = {
	.getattr	= f2fs_getattr,
	.setattr	= f2fs_setattr,
	.get_acl	= f2fs_get_acl,
#ifdef CONFIG_F2FS_FS_XATTR
	.setxattr	= generic_setxattr,
	.getxattr	= generic_getxattr,
	.listxattr	= f2fs_listxattr,
	.removexattr	= generic_removexattr,
#endif
<<<<<<< HEAD
	.fiemap		= f2fs_fiemap,
=======
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
};

static void fill_zero(struct inode *inode, pgoff_t index,
					loff_t start, loff_t len)
{
<<<<<<< HEAD
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
=======
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	struct page *page;

	if (!len)
		return;

	f2fs_balance_fs(sbi);

	f2fs_lock_op(sbi);
	page = get_new_data_page(inode, NULL, index, false);
	f2fs_unlock_op(sbi);

	if (!IS_ERR(page)) {
<<<<<<< HEAD
		f2fs_wait_on_page_writeback(page, DATA);
=======
		wait_on_page_writeback(page);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
		zero_user(page, start, len);
		set_page_dirty(page);
		f2fs_put_page(page, 1);
	}
}

int truncate_hole(struct inode *inode, pgoff_t pg_start, pgoff_t pg_end)
{
	pgoff_t index;
	int err;

	for (index = pg_start; index < pg_end; index++) {
		struct dnode_of_data dn;

		set_new_dnode(&dn, inode, NULL, NULL, 0);
		err = get_dnode_of_data(&dn, index, LOOKUP_NODE);
		if (err) {
			if (err == -ENOENT)
				continue;
			return err;
		}

		if (dn.data_blkaddr != NULL_ADDR)
			truncate_data_blocks_range(&dn, 1);
		f2fs_put_dnode(&dn);
	}
	return 0;
}

static int punch_hole(struct inode *inode, loff_t offset, loff_t len)
{
	pgoff_t pg_start, pg_end;
	loff_t off_start, off_end;
	int ret = 0;

<<<<<<< HEAD
	if (!S_ISREG(inode->i_mode))
		return -EOPNOTSUPP;

	/* skip punching hole beyond i_size */
	if (offset >= inode->i_size)
		return ret;

	if (f2fs_has_inline_data(inode)) {
		ret = f2fs_convert_inline_inode(inode);
		if (ret)
			return ret;
	}

=======
	ret = f2fs_convert_inline_data(inode, MAX_INLINE_DATA + 1);
	if (ret)
		return ret;

>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	pg_start = ((unsigned long long) offset) >> PAGE_CACHE_SHIFT;
	pg_end = ((unsigned long long) offset + len) >> PAGE_CACHE_SHIFT;

	off_start = offset & (PAGE_CACHE_SIZE - 1);
	off_end = (offset + len) & (PAGE_CACHE_SIZE - 1);

	if (pg_start == pg_end) {
		fill_zero(inode, pg_start, off_start,
						off_end - off_start);
	} else {
		if (off_start)
			fill_zero(inode, pg_start++, off_start,
					PAGE_CACHE_SIZE - off_start);
		if (off_end)
			fill_zero(inode, pg_end, 0, off_end);

		if (pg_start < pg_end) {
			struct address_space *mapping = inode->i_mapping;
			loff_t blk_start, blk_end;
<<<<<<< HEAD
			struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
=======
			struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4

			f2fs_balance_fs(sbi);

			blk_start = pg_start << PAGE_CACHE_SHIFT;
			blk_end = pg_end << PAGE_CACHE_SHIFT;
			truncate_inode_pages_range(mapping, blk_start,
					blk_end - 1);

			f2fs_lock_op(sbi);
			ret = truncate_hole(inode, pg_start, pg_end);
			f2fs_unlock_op(sbi);
		}
	}

	return ret;
}

static int expand_inode_data(struct inode *inode, loff_t offset,
					loff_t len, int mode)
{
<<<<<<< HEAD
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);
=======
	struct f2fs_sb_info *sbi = F2FS_SB(inode->i_sb);
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	pgoff_t index, pg_start, pg_end;
	loff_t new_size = i_size_read(inode);
	loff_t off_start, off_end;
	int ret = 0;

<<<<<<< HEAD
	f2fs_balance_fs(sbi);

=======
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	ret = inode_newsize_ok(inode, (len + offset));
	if (ret)
		return ret;

<<<<<<< HEAD
	if (f2fs_has_inline_data(inode)) {
		ret = f2fs_convert_inline_inode(inode);
		if (ret)
			return ret;
	}
=======
	ret = f2fs_convert_inline_data(inode, offset + len);
	if (ret)
		return ret;
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4

	pg_start = ((unsigned long long) offset) >> PAGE_CACHE_SHIFT;
	pg_end = ((unsigned long long) offset + len) >> PAGE_CACHE_SHIFT;

	off_start = offset & (PAGE_CACHE_SIZE - 1);
	off_end = (offset + len) & (PAGE_CACHE_SIZE - 1);

<<<<<<< HEAD
	f2fs_lock_op(sbi);

	for (index = pg_start; index <= pg_end; index++) {
		struct dnode_of_data dn;

		if (index == pg_end && !off_end)
			goto noalloc;

		set_new_dnode(&dn, inode, NULL, NULL, 0);
		ret = f2fs_reserve_block(&dn, index);
		if (ret)
			break;
noalloc:
=======
	for (index = pg_start; index <= pg_end; index++) {
		struct dnode_of_data dn;

		f2fs_lock_op(sbi);
		set_new_dnode(&dn, inode, NULL, NULL, 0);
		ret = f2fs_reserve_block(&dn, index);
		f2fs_unlock_op(sbi);
		if (ret)
			break;

>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
		if (pg_start == pg_end)
			new_size = offset + len;
		else if (index == pg_start && off_start)
			new_size = (index + 1) << PAGE_CACHE_SHIFT;
		else if (index == pg_end)
			new_size = (index << PAGE_CACHE_SHIFT) + off_end;
		else
			new_size += PAGE_CACHE_SIZE;
	}

	if (!(mode & FALLOC_FL_KEEP_SIZE) &&
		i_size_read(inode) < new_size) {
		i_size_write(inode, new_size);
		mark_inode_dirty(inode);
<<<<<<< HEAD
		update_inode_page(inode);
	}
	f2fs_unlock_op(sbi);
=======
	}
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4

	return ret;
}

static long f2fs_fallocate(struct file *file, int mode,
				loff_t offset, loff_t len)
{
<<<<<<< HEAD
	struct inode *inode = file_inode(file);
=======
	struct inode *inode = file->f_path.dentry->d_inode;
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	long ret;

	if (mode & ~(FALLOC_FL_KEEP_SIZE | FALLOC_FL_PUNCH_HOLE))
		return -EOPNOTSUPP;

<<<<<<< HEAD
	mutex_lock(&inode->i_mutex);

=======
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	if (mode & FALLOC_FL_PUNCH_HOLE)
		ret = punch_hole(inode, offset, len);
	else
		ret = expand_inode_data(inode, offset, len, mode);

	if (!ret) {
		inode->i_mtime = inode->i_ctime = CURRENT_TIME;
		mark_inode_dirty(inode);
	}
<<<<<<< HEAD

	mutex_unlock(&inode->i_mutex);

=======
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	trace_f2fs_fallocate(inode, mode, offset, len, ret);
	return ret;
}

#define F2FS_REG_FLMASK		(~(FS_DIRSYNC_FL | FS_TOPDIR_FL))
#define F2FS_OTHER_FLMASK	(FS_NODUMP_FL | FS_NOATIME_FL)

static inline __u32 f2fs_mask_flags(umode_t mode, __u32 flags)
{
	if (S_ISDIR(mode))
		return flags;
	else if (S_ISREG(mode))
		return flags & F2FS_REG_FLMASK;
	else
		return flags & F2FS_OTHER_FLMASK;
}

<<<<<<< HEAD
static int f2fs_ioc_getflags(struct file *filp, unsigned long arg)
{
	struct inode *inode = file_inode(filp);
	struct f2fs_inode_info *fi = F2FS_I(inode);
	unsigned int flags = fi->i_flags & FS_FL_USER_VISIBLE;
	return put_user(flags, (int __user *)arg);
}

static int f2fs_ioc_setflags(struct file *filp, unsigned long arg)
{
	struct inode *inode = file_inode(filp);
	struct f2fs_inode_info *fi = F2FS_I(inode);
	unsigned int flags = fi->i_flags & FS_FL_USER_VISIBLE;
	unsigned int oldflags;
	int ret;

	ret = mnt_want_write_file(filp);
	if (ret)
		return ret;

	if (!inode_owner_or_capable(inode)) {
		ret = -EACCES;
		goto out;
	}

	if (get_user(flags, (int __user *)arg)) {
		ret = -EFAULT;
		goto out;
	}

	flags = f2fs_mask_flags(inode->i_mode, flags);

	mutex_lock(&inode->i_mutex);

	oldflags = fi->i_flags;

	if ((flags ^ oldflags) & (FS_APPEND_FL | FS_IMMUTABLE_FL)) {
		if (!capable(CAP_LINUX_IMMUTABLE)) {
			mutex_unlock(&inode->i_mutex);
			ret = -EPERM;
			goto out;
		}
	}

	flags = flags & FS_FL_USER_MODIFIABLE;
	flags |= oldflags & ~FS_FL_USER_MODIFIABLE;
	fi->i_flags = flags;
	mutex_unlock(&inode->i_mutex);

	f2fs_set_inode_flags(inode);
	inode->i_ctime = CURRENT_TIME;
	mark_inode_dirty(inode);
out:
	mnt_drop_write_file(filp);
	return ret;
}

static int f2fs_ioc_start_atomic_write(struct file *filp)
{
	struct inode *inode = file_inode(filp);
	struct f2fs_sb_info *sbi = F2FS_I_SB(inode);

	if (!inode_owner_or_capable(inode))
		return -EACCES;

	f2fs_balance_fs(sbi);

	set_inode_flag(F2FS_I(inode), FI_ATOMIC_FILE);

	return f2fs_convert_inline_inode(inode);
}

static int f2fs_ioc_commit_atomic_write(struct file *filp)
{
	struct inode *inode = file_inode(filp);
	int ret;

	if (!inode_owner_or_capable(inode))
		return -EACCES;

	if (f2fs_is_volatile_file(inode))
		return 0;

	ret = mnt_want_write_file(filp);
	if (ret)
		return ret;

	if (f2fs_is_atomic_file(inode))
		commit_inmem_pages(inode, false);

	ret = f2fs_sync_file(filp, 0, LONG_MAX, 0);
	mnt_drop_write_file(filp);
	return ret;
}

static int f2fs_ioc_start_volatile_write(struct file *filp)
{
	struct inode *inode = file_inode(filp);

	if (!inode_owner_or_capable(inode))
		return -EACCES;

	set_inode_flag(F2FS_I(inode), FI_VOLATILE_FILE);

	return f2fs_convert_inline_inode(inode);
}

static int f2fs_ioc_fitrim(struct file *filp, unsigned long arg)
{
	struct inode *inode = file_inode(filp);
	struct super_block *sb = inode->i_sb;
	struct request_queue *q = bdev_get_queue(sb->s_bdev);
	struct fstrim_range range;
	int ret;

	if (!capable(CAP_SYS_ADMIN))
		return -EPERM;

	if (!blk_queue_discard(q))
		return -EOPNOTSUPP;

	if (copy_from_user(&range, (struct fstrim_range __user *)arg,
				sizeof(range)))
		return -EFAULT;

	range.minlen = max((unsigned int)range.minlen,
				q->limits.discard_granularity);
	ret = f2fs_trim_fs(F2FS_SB(sb), &range);
	if (ret < 0)
		return ret;

	if (copy_to_user((struct fstrim_range __user *)arg, &range,
				sizeof(range)))
		return -EFAULT;
	return 0;
}

long f2fs_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case F2FS_IOC_GETFLAGS:
		return f2fs_ioc_getflags(filp, arg);
	case F2FS_IOC_SETFLAGS:
		return f2fs_ioc_setflags(filp, arg);
	case F2FS_IOC_START_ATOMIC_WRITE:
		return f2fs_ioc_start_atomic_write(filp);
	case F2FS_IOC_COMMIT_ATOMIC_WRITE:
		return f2fs_ioc_commit_atomic_write(filp);
	case F2FS_IOC_START_VOLATILE_WRITE:
		return f2fs_ioc_start_volatile_write(filp);
	case FITRIM:
		return f2fs_ioc_fitrim(filp, arg);
=======
long f2fs_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct inode *inode = filp->f_dentry->d_inode;
	struct f2fs_inode_info *fi = F2FS_I(inode);
	unsigned int flags;
	int ret;

	switch (cmd) {
	case F2FS_IOC_GETFLAGS:
		flags = fi->i_flags & FS_FL_USER_VISIBLE;
		return put_user(flags, (int __user *) arg);
	case F2FS_IOC_SETFLAGS:
	{
		unsigned int oldflags;

		ret = mnt_want_write(filp->f_path.mnt);
		if (ret)
			return ret;

		if (!inode_owner_or_capable(inode)) {
			ret = -EACCES;
			goto out;
		}

		if (get_user(flags, (int __user *) arg)) {
			ret = -EFAULT;
			goto out;
		}

		flags = f2fs_mask_flags(inode->i_mode, flags);

		mutex_lock(&inode->i_mutex);

		oldflags = fi->i_flags;

		if ((flags ^ oldflags) & (FS_APPEND_FL | FS_IMMUTABLE_FL)) {
			if (!capable(CAP_LINUX_IMMUTABLE)) {
				mutex_unlock(&inode->i_mutex);
				ret = -EPERM;
				goto out;
			}
		}

		flags = flags & FS_FL_USER_MODIFIABLE;
		flags |= oldflags & ~FS_FL_USER_MODIFIABLE;
		fi->i_flags = flags;
		mutex_unlock(&inode->i_mutex);

		f2fs_set_inode_flags(inode);
		inode->i_ctime = CURRENT_TIME;
		mark_inode_dirty(inode);
out:
		mnt_drop_write(filp->f_path.mnt);
		return ret;
	}
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	default:
		return -ENOTTY;
	}
}

#ifdef CONFIG_COMPAT
long f2fs_compat_ioctl(struct file *file, unsigned int cmd, unsigned long arg)
{
	switch (cmd) {
	case F2FS_IOC32_GETFLAGS:
		cmd = F2FS_IOC_GETFLAGS;
		break;
	case F2FS_IOC32_SETFLAGS:
		cmd = F2FS_IOC_SETFLAGS;
		break;
	default:
		return -ENOIOCTLCMD;
	}
	return f2fs_ioctl(file, cmd, (unsigned long) compat_ptr(arg));
}
#endif

const struct file_operations f2fs_file_operations = {
<<<<<<< HEAD
	.llseek		= f2fs_llseek,
=======
	.llseek		= generic_file_llseek,
>>>>>>> f674d0881c3ecec6016d7aa8b91132f1d40432d4
	.read		= do_sync_read,
	.write		= do_sync_write,
	.aio_read	= generic_file_aio_read,
	.aio_write	= generic_file_aio_write,
	.open		= generic_file_open,
	.mmap		= f2fs_file_mmap,
	.fsync		= f2fs_sync_file,
	.fallocate	= f2fs_fallocate,
	.unlocked_ioctl	= f2fs_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl	= f2fs_compat_ioctl,
#endif
	.splice_read	= generic_file_splice_read,
	.splice_write	= generic_file_splice_write,
};
